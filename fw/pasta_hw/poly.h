#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"
#include "symmetric.h"

typedef struct {
  int32_t coeffs[N];
} poly;


void poly_add(poly *c, const poly *a, const poly *b);
void poly_sub(poly *c, const poly *a, const poly *b);
void poly_ntt(poly *a);
void poly_invntt(poly *a);
void poly_pointwise(poly *c, const poly *a, const poly *b);
void poly_pointwise_acc(poly *c, const poly *a, const poly *b);
void poly_init_q(void);
void poly_set_q(void);
void poly_invntt_sub(poly *a, poly *b, poly *c);

int poly_pointwise_add_invntt_chknorm(poly *r, const poly *v, const poly *c, const poly *u, uint32_t B);
int poly_pointwise_invntt_sub_chknorm(poly *r, const poly *v, const poly *c, const poly *u, uint32_t B);
int poly_invntt_chknorm(poly *a, uint32_t B);
void poly_uniform(poly *a, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr, int allow_zero, int to_hw);


#endif
