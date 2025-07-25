#include "masked.h"




void mask_seed(masked_seed dst, const uint8_t src[SEEDBYTES])
{
    int i, j;
    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < SEEDBYTES; j++) {
            if (i == 0) {
                dst[i][j] = src[j] ^ j;
            }
            else {
                dst[i][j] = j;
            }
        }
    }
}