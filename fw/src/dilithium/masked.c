#include <string.h>
#include "masked.h"
#include "x2x.h"



void mask_seed(masked_seed dst, const uint8_t src[SEEDBYTES])
{
    int i, j;
    uint32_t rnd[16];
    uint8_t *rnd_ptr = (uint8_t*) rnd;

    x2x_prng_read(rnd, 16);

    for (j = 0; j < SEEDBYTES; j++) {
        dst[0][j] = src[j] ^ rnd_ptr[j];
    }

    for (i = 1; i < MASKING_N; i++) {
        memcpy(dst[i], rnd_ptr, SEEDBYTES);
    }
}