#ifndef MASKED_H
#define MASKED_H


#include <stdint.h>
#include "params.h"

#define MASKING_N         2
#define LOG_MASKING_N     1


typedef uint8_t masked_seed[MASKING_N][SEEDBYTES];

typedef uint8_t masked_crh[MASKING_N][CRHBYTES];

typedef uint8_t *masked_flat_ptr;


#define mask_seed DILITHIUM_NAMESPACE(mask_seed)
void mask_seed(masked_seed dst, const uint8_t src[SEEDBYTES]);


#endif