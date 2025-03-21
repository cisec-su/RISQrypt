#ifndef RNG_H
#define RNG_H


#include "stdint.h"


int randombytes(unsigned char *x, unsigned long long xlen);

uint16_t rand16();


#endif
