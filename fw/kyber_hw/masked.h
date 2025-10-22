#ifndef MASKED_H
#define MASKED_H


#include <stdint.h>
#include "params.h"

#define MASKING_N         2
#define LOG_MASKING_N     1

#define FLAT_TO_PTR(SRC) {(SRC)[0], (SRC)[1]} 

typedef uint8_t masked_msg[MASKING_N][KYBER_INDCPA_MSGBYTES];

typedef uint8_t masked_sym[MASKING_N][KYBER_SYMBYTES];

typedef uint8_t masked_ss[MASKING_N][KYBER_SSBYTES];

typedef uint8_t *masked_ptr[MASKING_N];

typedef uint32_t masked_u32[MASKING_N];

#endif