#ifndef RNG_H
#define RNG_H


#include "stdint.h"


int randombytes(uint8_t *x, unsigned int xlen);

void rng_init();

#endif
