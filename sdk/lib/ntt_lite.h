#ifndef NTT_LITE_H
#define NTT_LITE_H


#include <stdint.h>


#define NTT_LITE_MODE_SINGLE      ((uint32_t) 0x0)
#define NTT_LITE_MODE_DUAL        ((uint32_t) 0x2)
#define NTT_LITE_MODE_POLY        ((uint32_t) 0x3)

#define NTT_LITE_INPUT_DIS        ((uint32_t*) 0x0)
#define NTT_LITE_OUTPUT_DIS       ((uint32_t*) 0x0)


int ntt_lite_load_q(uint32_t q, const uint32_t *mu, uint32_t logn, uint32_t logq, uint32_t inv2, uint32_t mode);

int ntt_lite_set_mode(uint32_t mode);

int ntt_lite_load_twiddle(const uint32_t *psi);

int ntt_lite_forward_ntt(uint32_t *dst,  const uint32_t *src);

int ntt_lite_backward_ntt(uint32_t *dst,  const uint32_t *src);

int ntt_lite_pwm(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs);

int ntt_lite_add(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs);

int ntt_lite_sub(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs);

int ntt_lite_square(uint32_t *dst, const uint32_t *src);

int ntt_lite_sum(uint32_t* dst, const uint32_t *src);

int ntt_lite_encode(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_decode(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_compress(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_decompress(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_decompress_floor(uint32_t *dst, const uint32_t *src, uint32_t d);


#endif