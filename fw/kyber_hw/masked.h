#ifndef MASKED_H
#define MASKED_H


#include <stdint.h>
#include "params.h"

#define MASKING_N         2
#define LOG_MASKING_N     1


typedef uint8_t masked_msg[MASKING_N][KYBER_INDCPA_MSGBYTES];

typedef uint8_t masked_sym[MASKING_N][KYBER_SYMBYTES];

typedef uint8_t *masked_ptr[MASKING_N];

#endif