/*
// Author: Jiasheng.chen@celestialsemi.com
// Version0.1 Inital @2008.02.14
// Ported by Santosh Shukla in u-boot
*/

#include <common.h>
#include <rsa_verify.h>
/*#include "sha2.h"*/
#include <sha256.h>
#include <rsa.h>
#include <c_stdlib.h>
//#include "secu_hw_if.h" // commented due to compilation issue

enum _SECU_BOOT_CONST_
{
	SECU_SHA256_LEN    = 32,
	SECU_RSA_KEY_N_LEN = 2048/8,
	SECU_RSA_SIG_LEN   = SECU_RSA_KEY_N_LEN,
	SECU_RSA_KEY_E_LEN = 4,
};

static unsigned char ExternalRSAKeyN[SECU_RSA_KEY_N_LEN];
static unsigned char ExternalRSAKeyE[SECU_RSA_KEY_E_LEN] = {0x00, 0x1, 0x00, 0x1};

static int BlockHashImg(unsigned char *InputImg, int Size, unsigned char *Hash);
static int RsaVerifySig(unsigned char *hash, 
                        unsigned char *SigBuf);

void UpdateExternalRSAKey(unsigned char *InputBuf)
{
    memcpy(ExternalRSAKeyN, InputBuf, SECU_RSA_KEY_N_LEN);
    
    return;
}

////////////////////////////////////////////////////////////////////////////////
///
/// \Fn			    SHAVerify
/// \Brief		    signature verification
///				    1. hash the image 
///                 2. compare signature
/// \Param[in]	    
///                 InputBuf - input data
///                 Size     - data size
///                 SigBuf   - signature data
/// \Param[in/out]  
/// \Param[out]	    
/// \Exception	
/// \Return	
///                 0 - success, others are failed
///
////////////////////////////////////////////////////////////////////////////////
int SHAVerify(unsigned char *InputBuf, 
              int Size, 
              unsigned char *SigBuf)
{	
    int rt = -1;
    volatile unsigned int step = 0;
	unsigned char Hash[SECU_SHA256_LEN];
	
    rt = BlockHashImg(InputBuf, Size, Hash);
    step = (step << 1) | 0x1;
    if (rt != 0)
        return rt;
       
    /* verify SHA256 */
    rt = memcmp( Hash, SigBuf, SECU_SHA256_LEN );
    step = (step << 1) | 0x1;
    if (rt != 0)
        return rt;

    if (step == 0x3)
        return rt;
    else
        return -1;
}

////////////////////////////////////////////////////////////////////////////////
///
/// \Fn		    SignatureVerify
/// \Brief	    RSA decryption and signature verification
///		    1. hash the image 
///                 2. RSA decrypt signature 
///                 3. compare signature
/// \Param[in]	    
///                 InputBuf - input data
///                 Size     - data size
///                 SigBuf   - signature data
/// \Param[in/out]  
/// \Param[out]	    
/// \Exception	
/// \Return	
///                 0 - success, others are failed
///
////////////////////////////////////////////////////////////////////////////////
int SignatureVerify(unsigned char *InputBuf, 
				int Size, 
			unsigned char *SigBuf)
{
	/* stack 8704 bytes*/
	int rt = -1;    
	volatile unsigned int step = 0;
	unsigned char Hash[SECU_SHA256_LEN];

	rt = BlockHashImg(InputBuf, Size, Hash);
	step = (step << 1) | 0x1;
	if (rt != 0)
		return rt;

	rt = RsaVerifySig(Hash, SigBuf);
	step = (step << 1) | 0x1;
	if (rt != 0)
		return rt;

	if (step == 0x3)
		return rt;
	else
		return -1;
}

/* hybrid hash rsa verification method API */
int HybridSignatureVerify( unsigned char hash[32], unsigned char *SigBuf)
{
	/* stack 8704 bytes*/
	int rt = -1;    
	volatile unsigned int step = 0;

	/* dummy hashing approval method */
	step = (step << 1) | 0x1;

	/* rsa verify */
	rt = RsaVerifySig(hash, SigBuf);
	step = (step << 1) | 0x1;
	if (rt != 0)
		return rt;

	if (step == 0x3)
		return rt;
	else
		return -1;
}

static int BlockHashImg(unsigned char *InputImg, int Size, unsigned char *Hash)
/*
 * Hash calculation for memory block
 * Return True or False
 */    
{
    int rt = -1;

    /*rt = sha2(InputImg, Size, Hash, 0);*/
    rt = sha256(InputImg, Size, Hash);

    return rt;
}

static int RsaVerifySig(unsigned char *hash, 
                        unsigned char *SigBuf)
{
	/* stack 8664 bytes */
	int ret = -1;
	volatile unsigned int step = 0;

	rsa_context rsa;
	// 0 Initial
	rsa_init( &rsa, RSA_PKCS_V15, 0, NULL, NULL );
	step = (step << 1) | 0x1;       // step 1 done
	// 1 Read PubKey
	{
		unsigned char *KeyNBuf;
		unsigned char *KeyEBuf;
		int KeyNBufLen = 0;
		int KeyEBufLen = 0;

		KeyNBuf = ExternalRSAKeyN;
		KeyNBufLen = sizeof(ExternalRSAKeyN) / sizeof(unsigned char);

		KeyEBuf = ExternalRSAKeyE;
		KeyEBufLen = sizeof(ExternalRSAKeyE) / sizeof(unsigned char);

		if (KeyNBufLen != SECU_RSA_KEY_N_LEN || 
		    KeyEBufLen != SECU_RSA_KEY_E_LEN)
		{
		    ret = -1;
		    goto exit;
		}
		
		if( ( ret = mpi_read_binary( &rsa.N, KeyNBuf, KeyNBufLen ))||
			( ret = mpi_read_binary( &rsa.E, KeyEBuf, KeyEBufLen )))
		{
			ret = -1;
			goto exit;
		}
		rsa.len = ( mpi_msb( &rsa.N ) + 7 ) >> 3;
	}
	step = (step << 1) | 0x1;           // step 2 done
	// 2 check rsa key
	{
		if ( ( ret = rsa_check_pubkey(&rsa) ) != 0 )
		{
		    ret = -1;
		    goto exit; 
		}
	}
		step = (step << 1) | 0x1;           // step 3 done

	// 3  Do Verify
	{         
		ret = rsa_pkcs1_verify( &rsa, RSA_PUBLIC, RSA_SHA256,
				SECU_SHA256_LEN, hash, SigBuf);

		if( ret != 0 )
		{
		    ret = -1;
		    goto exit;
		}
	}
	step = (step << 1) | 0x1;           // step 4 done

exit:
	rsa_free(&rsa);
	if (step == 0xf)   	            // step check
		return ret;
	else
		return -1;
}
