#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"
#include "symmetric.h"

typedef struct {
  int16_t coeffs[N];
} poly;

#define poly_init_q FALCON_NAMESPACE(poly_init_q)
void poly_init_q(void);

#define poly_init_ntt FALCON_NAMESPACE(poly_init_ntt)
void poly_init_ntt(void);

#define poly_init_invntt FALCON_NAMESPACE(poly_init_invntt)
void poly_init_invntt(void);

#define poly_ntt FALCON_NAMESPACE(poly_ntt)
void poly_ntt(poly *a);

#define poly_ntt_from_center FALCON_NAMESPACE(poly_ntt_from_center)
void poly_ntt_from_center(poly *a, const poly *b);

#define poly_invntt_sub FALCON_NAMESPACE(poly_invntt_sub)
void poly_invntt_sub(poly *a, const poly *b);

#define poly_basemul FALCON_NAMESPACE(poly_basemul)
void poly_basemul(poly *r, const poly *a, const poly *b);

#define poly_hash_to_point FALCON_NAMESPACE(poly_hash_to_point)
void poly_hash_to_point(poly *x);

#define poly_is_short FALCON_NAMESPACE(poly_is_short)
int poly_is_short(const poly *s1, const poly *s2);

#define poly_modq_decode FALCON_NAMESPACE(poly_modq_decode)
int poly_modq_decode(poly *r, const void *in, size_t max_in_len);

#define poly_comp_decode FALCON_NAMESPACE(poly_comp_decode)
int poly_comp_decode(poly *r, const void *in, size_t max_in_len);

#endif
