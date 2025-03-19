#ifndef MASKED_H
#define MASKED_H


#include <stdint.h>
#include "params.h"
#include "masked_poly.h"

#define MASKING_N         2
#define LOG_MASKING_N     1


typedef uint8_t masked_msg[MASKING_N][KYBER_INDCPA_MSGBYTES];



#endif