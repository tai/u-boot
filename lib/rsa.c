/* 
 * Copyright (c) 2006-2007, Christophe Devine
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of the XySSL nor the names of its contributors
 *       may be used to endorse or promote products derived from this
 *       software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  RSA was designed by Ron Rivest, Adi Shamir and Len Adleman.
 *
 *  http://theory.lcs.mit.edu/~rivest/rsapaper.pdf
 *  http://www.cacr.math.uwaterloo.ca/hac/about/chap8.pdf
 */
#include <common.h>
#include <config_xyssl.h>

#if defined(XYSSL_RSA_C)

#include <rsa.h>
#include <c_stdlib.h>



/*
 * Initialize an RSA context
 */
void rsa_init( rsa_context *ctx,
               int padding,
               int hash_id,
               int (*f_rng)(void *),
               void *p_rng )
{
	mpi *Ts[3];
    
	//memset( ctx, 0, sizeof( rsa_context ) );
	ctx->len = 0;
	ctx->ver = 0;

    ctx->padding = padding;
    ctx->hash_id = hash_id;

    ctx->f_rng = f_rng;
    ctx->p_rng = p_rng;

	
	Ts[0] = &ctx->E;
	Ts[1] = &ctx->N;
	Ts[2] = &ctx->RN;
	mpi_init( Ts, 3);    
}


/*
 * Check a public RSA key
 */
int rsa_check_pubkey( rsa_context *ctx )
{
    if( ( ctx->N.p[0] & 1 ) == 0 || 
        ( ctx->E.p[0] & 1 ) == 0 )
        return( XYSSL_ERR_RSA_KEY_CHECK_FAILED );

    if( mpi_msb( &ctx->N ) != RSA_KEY_N_SIZE )
        return( XYSSL_ERR_RSA_KEY_CHECK_FAILED );

    if( mpi_msb( &ctx->E ) < 2 ||
        mpi_msb( &ctx->E ) > RSA_KEY_E_SIZE )
        return( XYSSL_ERR_RSA_KEY_CHECK_FAILED );

    return( 0 );
}


/*
 * Do an RSA public key operation
 */
int rsa_public( rsa_context *ctx,
                unsigned char *input,
                unsigned char *output )
{
    int ret, olen;
    mpi T;
    mpi *Ts[1];

    //mpi_init( &T, NULL );
    Ts[0] = &T;
	mpi_init( Ts, 1);


    MPI_CHK( mpi_read_binary( &T, input, ctx->len ) );

    if( mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        //mpi_free( &T, NULL );
        {
			Ts[0] = &T;
			mpi_free( Ts, 1);
	    }
        return( XYSSL_ERR_RSA_BAD_INPUT_DATA );
    }

    olen = ctx->len;
    MPI_CHK( mpi_exp_mod( &T, &T, &ctx->E, &ctx->N, &ctx->RN ) );
    MPI_CHK( mpi_write_binary( &T, output, olen ) );

cleanup:

    //mpi_free( &T, NULL );
	{
		Ts[0] = &T;
		mpi_free( Ts, 1);
    }
    if( ret != 0 )
        return( XYSSL_ERR_RSA_PUBLIC_FAILED | ret );

    return( 0 );
}
/*
 * Do an RSA operation and check the message digest
 */
//#define DEBUG_TRACE
#ifdef DEBUG_TRACE
#include <stdio.h>
#endif
int rsa_pkcs1_verify( rsa_context *ctx,
                      int mode,
                      int hash_id,
                      int hashlen,
                      unsigned char *hash,
                      unsigned char *sig )
{
    int ret, len, siglen;
    unsigned char *p;
    unsigned char buf[RSA_KEY_N_SIZE>>3];

    siglen = ctx->len;

    if( siglen < 16 || siglen > (int) sizeof( buf ) )
        return( XYSSL_ERR_RSA_BAD_INPUT_DATA );


    ret = ( mode == RSA_PUBLIC )
          ? rsa_public(  ctx, sig, buf )
          : rsa_public(  ctx, sig, buf );
          //: rsa_private( ctx, sig, buf );              
          
    if( ret != 0 )
        return( ret );

    p = buf;
	#ifdef DEBUG_TRACE
	{
		FILE *fp = NULL;
		fp = fopen("rsa_sign_new.log", "wb+"); assert(fp != NULL);
		fwrite(buf, sizeof(unsigned char), siglen, fp);
		fclose(fp);fp = NULL;
	}
	#endif
    switch( ctx->padding )
    {
        case RSA_PKCS_V15:

            if( *p++ != 0 || *p++ != RSA_SIGN )
                return( XYSSL_ERR_RSA_INVALID_PADDING );

            while( *p != 0 )
            {
                if( p >= buf + siglen - 1 || *p != 0xFF )
                    return( XYSSL_ERR_RSA_INVALID_PADDING );
                p++;
            }
            p++;
            break;

        default:

            return( XYSSL_ERR_RSA_INVALID_PADDING );
    }

    len = siglen - (int)( p - buf );
	
	//For SHA256
	if(hash_id == RSA_SHA256 )
    	{
        	if( memcmp( p, ASN1_HASH_SHA256, 19 ) == 0 &&
            		memcmp( p + 19, hash, 32 ) == 0 )
            		return( 0 );
        	else
            		return( XYSSL_ERR_RSA_VERIFY_FAILED );
    	}
	else
	{
		return (XYSSL_ERR_RSA_NOT_SUPPORTED);
	}   
    
	return( XYSSL_ERR_RSA_INVALID_PADDING );
}

/*
 * Free the components of an RSA key
 */
void rsa_free( rsa_context *ctx )
{
	/*
		//mpi_free( &ctx->RQ, &ctx->RP, &ctx->RN,
		&ctx->QP, &ctx->DQ, &ctx->DP,
		&ctx->Q,  &ctx->P,  &ctx->D,
		&ctx->E,  &ctx->N,  NULL );
	*/
	{
		/*
		mpi *Ts[13];
		Ts[0] = &ctx->RQ;
		Ts[1] = &ctx->RP;
		Ts[2] = &ctx->RQ;
		Ts[3] = &ctx->RP;
		Ts[4] = &ctx->RN;
		Ts[5] = &ctx->QP;
		Ts[6] = &ctx->DQ;
		Ts[7] = &ctx->DP;
		Ts[8] = &ctx->Q;
		Ts[9] = &ctx->P;
		Ts[10] = &ctx->D;
		Ts[11] = &ctx->E;
		Ts[12] = &ctx->N;
		mpi_free( Ts, 13);
		*/
		mpi *Ts[3];
		Ts[0] = &ctx->E;
		Ts[1] = &ctx->N;
		Ts[2] = &ctx->RN;
		mpi_free( Ts, 3);
	}
}


#endif
