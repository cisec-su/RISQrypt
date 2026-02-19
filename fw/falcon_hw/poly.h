#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"
#include "symmetric.h"

typedef struct {
  int16_t coeffs[N];
} poly;

void poly_init_q(void);
void poly_set_q(void);
void poly_init_ntt(void);
void poly_init_invntt(void);
void poly_reduce(poly *a);

void poly_basemul(poly *r, const poly *a, const poly *b);

#endif
