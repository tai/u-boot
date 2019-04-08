/**
 * \file config.h
 *
 * This set of compile-time options may be used to enable
 * or disable features selectively, and reduce the global
 * memory footprint.
 */
#ifndef XYSSL_CONFIG_H
#define XYSSL_CONFIG_H

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

/*
 * Uncomment if native integers are 8-bit wide.
 *
#define XYSSL_HAVE_INT8
 */

/*
 * Uncomment if native integers are 16-bit wide.
 *
#define XYSSL_HAVE_INT16
 */

/*
 * Uncomment if the compiler supports long long.
 *
#define XYSSL_HAVE_LONGLONG
 */

/*
 * Uncomment if the CPU supports SSE2 (IA-32 specific).
 *
#define XYSSL_HAVE_SSE2
 */


/*
 * Enable the checkup functions (*_self_test).
 */
//#define XYSSL_SELF_TEST

/*
 * Enable the prime-number generation code.
 */
//#define XYSSL_GENPRIME


/*
 * Module:  library/bignum.c
 * Caller:  library/dhm.c
 *          library/rsa.c
 *          library/ssl_tls.c
 *          library/x509parse.c
 *
 * This module is required for RSA and DHM support.
 */
#define XYSSL_BIGNUM_C



/*
 * Module:  library/rsa.c
 * Caller:  library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *          library/x509.c
 *
 * This module is required for SSL/TLS and X.509.
 */
#define XYSSL_RSA_C


/*
 * Module:  library/sha2.c
 * Caller:
 *
 * This module adds support for SHA-224 and SHA-256.
 */
#define XYSSL_SHA2_C

//#define BIGNUM_FILE_FUNC_ENA

#endif /* config.h */
