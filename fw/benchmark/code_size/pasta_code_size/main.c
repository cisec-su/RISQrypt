#include <stdint.h>
#include "pasta.h"
#include "params.h"
#ifdef MASKING_EN
#include "x2x.h"
#include "masked_pasta.h"
#endif


void test() {
    int32_t key[PASTA_KEY_SIZE];
    poly plaintext, ciphertext;
    uint64_t nonce;
    uint64_t block_ctr;

    pasta_key_gen(key, &nonce, &block_ctr);
    (void) plaintext;
    (void) nonce;
    (void) block_ctr;

#ifdef MASKING_EN
    masked_pasta_encrypt(&ciphertext, &plaintext, key, nonce);
#else
    pasta_encrypt(&ciphertext, &plaintext, key, nonce);
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
