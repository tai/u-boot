#include <common.h>
#include <sha256.h>
#include <rsa.h>
#include <rsa_verify.h>

unsigned char default_rsa_key_e[SECU_RSA_KEY_E_LEN] = {0x00, 0x1, 0x00, 0x1};

void calculate_sha256(unsigned char *buf, int length, unsigned char *digest)
{
	sha256_context sha256;
	sha256_starts(&sha256);
	sha256_update(&sha256, buf, length);
	sha256_finish(&sha256, digest);
}

int rsa_verify_sig(unsigned char *rsa_key_n, unsigned char *rsa_key_e, unsigned char *hash, unsigned char *sig)
{
	int ret;
	rsa_context rsa;
	rsa_init(&rsa, RSA_PKCS_V15, 0);
	mpi_read_binary(&rsa.N, rsa_key_n, SECU_RSA_KEY_N_LEN);
	mpi_read_binary(&rsa.E, rsa_key_e, SECU_RSA_KEY_E_LEN);
	rsa.len = (mpi_msb(&rsa.N) + 7) >> 3;
	if((ret = rsa_check_pubkey(&rsa)) != 0)return ret;
	ret = rsa_pkcs1_verify(&rsa, RSA_PUBLIC, SIG_RSA_SHA256, SECU_SHA256_LEN, hash, sig);
	rsa_free(&rsa);
	return ret;
}


