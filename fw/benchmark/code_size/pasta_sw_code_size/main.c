#include <stdint.h>
#include <stddef.h>
#include "pasta_soft.h"
#include "params.h"
#ifdef MASKING_EN
#include "x2x.h"
#include "masked_pasta_soft.h"
#endif

// Constants required by pasta_soft
const uint32_t PASTA_KEY[2*N] = {0};
const uint32_t PASTA_PLAINTEXT[N] = {0};
const uint64_t PASTA_NONCE = 0x123456789ULL;
const uint64_t PASTA_BLOCK_CTR = 0x0ULL;

void test() {
    int32_t key[PASTA_KEY_SIZE];
    poly plaintext, ciphertext;
    uint64_t nonce;
    uint64_t block_ctr;

    pasta_soft_key_gen(key, &nonce, &block_ctr);
    (void) plaintext;
    (void) nonce;
    (void) block_ctr;

#ifdef MASKING_EN
    masked_pasta_soft_encrypt(&ciphertext, &plaintext, key, nonce);
#else
    pasta_soft_encrypt(&ciphertext, &plaintext, key, nonce);
#endif
    (void) ciphertext;
}


int main() {
#ifdef MASKING_EN
    uint32_t seed[2] = {1, 1};
    x2x_seed(seed);
#endif
    test();
}
