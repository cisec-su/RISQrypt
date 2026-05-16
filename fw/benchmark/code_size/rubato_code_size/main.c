#include <stdint.h>
#include "rubato.h"
#include "params.h"
#ifdef MASKING_EN
#include "x2x.h"
#include "masked_rubato.h"
#include "rubato.h"
#endif


void test() {
    poly key;
    poly plaintext, ciphertext;
    uint64_t nonce;
    uint64_t block_ctr;

    rubato_key_gen(&key, &nonce, &block_ctr);
    (void) plaintext;
    (void) nonce;
    (void) block_ctr;

#ifdef MASKING_EN
    masked_rubato_encrypt(&ciphertext, &plaintext, &key, nonce, block_ctr);
#else
    rubato_encrypt(&ciphertext, &plaintext, &key, nonce, block_ctr);
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
