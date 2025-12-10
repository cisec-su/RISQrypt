#ifndef X2X_H
#define X2X_H


#include <stdint.h>


#define X2X_MODULUS_POW2   ((uint32_t) 0x0)
#define X2X_MODULUS_PRIME  ((uint32_t) 0x1)

#define X2X_DUAL_MODE_EN   ((uint32_t) 0x1)
#define X2X_DUAL_MODE_DIS  ((uint32_t) 0x0)

#define X2X_REJ_SAMPLE_EN  ((uint32_t) 0x1)
#define X2X_REJ_SAMPLE_DIS ((uint32_t) 0x0)


int x2x_set_modulus(uint32_t modulus, uint32_t log_modulus, uint32_t modulus_type, uint32_t dual_mode, uint32_t rej_sample);

int x2x_seed(uint32_t seed[2]);

int x2x_a2b(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src0, unsigned int len);

int x2x_b2a(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src0, unsigned int len);

int x2x_b2a_1bit(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, uint32_t log_stride, unsigned int len);

int x2x_b_share(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src, unsigned int len);

int x2x_a_share(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src, unsigned int len);

int x2x_b_ref(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len);

int x2x_a_ref(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len);

int x2x_ref_b2a(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len);

int x2x_ref_b2a_1bit(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, uint32_t log_stride, unsigned int len);

int x2x_ref_a2b(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len);

int x2x_prng_read(uint32_t *dst, unsigned int len);

int x2x_prng_read_nonzero(uint32_t *dst, unsigned int len);


#endif