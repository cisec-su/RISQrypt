#ifndef X2X_H
#define X2X_H


#include <stdint.h>


#define X2X_MODULUS_POW2   ((uint32_t) 0)
#define X2X_MODULUS_PRIME  ((uint32_t) 1)


int x2x_set_modulus(uint32_t *modulus, unsigned int modulus_type);

int x2x_seed(uint32_t *seed);

int x2x_a2b(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src0, unsigned int len);

int x2x_b2a(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src0, unsigned int len);

int x2x_b_mask(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src, unsigned int len);

int x2x_a_mask(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src, unsigned int len);


#endif