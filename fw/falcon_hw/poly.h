#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"
#include "symmetric.h"

typedef struct {
  int32_t coeffs[N];
} poly;


// #define poly_add FALCON_NAMESPACE(poly_add)
// void poly_add(poly *c, const poly *a, const poly *b);
// #define poly_sub FALCON_NAMESPACE(poly_sub)
// void poly_sub(poly *c, const poly *a, const poly *b);

// #define poly_ntt FALCON_NAMESPACE(poly_ntt)
// void poly_ntt(poly *a);
// #define poly_invntt FALCON_NAMESPACE(poly_invntt)
// void poly_invntt(poly *a);
// #define poly_pointwise FALCON_NAMESPACE(poly_pointwise)
// void poly_pointwise(poly *c, const poly *a, const poly *b);
// #define poly_pointwise_acc FALCON_NAMESPACE(poly_pointwise_acc)
// void poly_pointwise_acc(poly *c, const poly *a, const poly *b);

// #define poly_init_q FALCON_NAMESPACE(poly_init_q)
// void poly_init_q(void);

// #define poly_set_q FALCON_NAMESPACE(poly_set_q)
// void poly_set_q(void);

// #define poly_init_ntt FALCON_NAMESPACE(poly_init_ntt)
// void poly_init_ntt(void);

// #define poly_init_invntt FALCON_NAMESPACE(poly_init_invntt)
// void poly_init_invntt(void);

// #define poly_invntt_sub FALCON_NAMESPACE(poly_invntt_sub)
// void poly_invntt_sub(poly *a, poly *b, poly *c);

// #define poly_basemul FALCON_NAMESPACE(poly_basemul)
// void poly_basemul(poly *r, const poly *a, const poly *b)

#endif
