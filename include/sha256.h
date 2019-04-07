#ifndef _SHA256_H
#define _SHA256_H

#define SHA256_SUM_LEN	32

typedef struct {
	uint32_t total[2];
	uint32_t state[8];
	uint8_t buffer[64];
} sha256_context;

void sha256_starts(sha256_context * ctx);
void sha256_update(sha256_context * ctx, uint8_t * input, uint32_t length);
void sha256_finish(sha256_context * ctx, uint8_t digest[SHA256_SUM_LEN]);

/**
 * \brief          Output = SHA-256( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
int  sha256( unsigned char *input, int ilen,
                unsigned char output[32]);

#endif /* _SHA256_H */
