#ifndef MASKED_H
#define MASKED_H


#include <stdint.h>
#include "params.h"


#define LOG_MASKING_N     1
#define MASKING_N         (1 << LOG_MASKING_N)



typedef uint8_t masked_msg[MASKING_N][KYBER_INDCPA_MSGBYTES];


#endif