#ifndef _RSA_VERIFY_H
#define _RSA_VERIFY_H
#define SECU_SHA256_LEN  32
#define SECU_RSA_KEY_N_LEN  2048/8
#define SECU_RSA_SIG_LEN SECU_RSA_KEY_N_LEN
#define SECU_RSA_KEY_E_LEN 4 

void calculate_sha256(unsigned char *buf, int length, unsigned char *digest);
int rsa_verify_sig(unsigned char *rsa_key_n, unsigned char *rsa_key_e, unsigned char *hash, unsigned char *sig);

#endif
