#ifndef RANDOMBYTES_H
#define RANDOMBYTES_H
#include <stdint.h>


#include "stdint.h"

void rng_init();

int randombytes(uint8_t *x, unsigned int xlen);


#endif
