#include "masked.h"
#include "x2x.h"



void mask_seed(masked_seed dst, const uint8_t src[SEEDBYTES])
{
    int i, j;
    uint32_t rnd[16];
    uint8_t *rnd_ptr = (uint8_t*) rnd;

    x2x_prng_read(rnd, 16);


    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < SEEDBYTES; j++) {
            if (i == 0) {
                dst[i][j] = src[j] ^ rnd_ptr[j];
            }
            else {
                dst[i][j] = rnd_ptr[j];
            }
        }
    }
}