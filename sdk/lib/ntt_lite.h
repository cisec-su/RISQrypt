#ifndef NTT_LITE_H
#define NTT_LITE_H


#include <stdint.h>


#define NTT_LITE_MODE_SINGLE         ((uint32_t) 0x0)
#define NTT_LITE_MODE_DUAL           ((uint32_t) 0x2)
#define NTT_LITE_MODE_POLY           ((uint32_t) 0x3)

#define NTT_LITE_INPUT_DIS           ((uint32_t*) 0x0)
#define NTT_LITE_OUTPUT_DIS          ((uint32_t*) 0x0)

#define NTT_LITE_CHKNORM_SUCC        ((uint32_t) 0x0)
#define NTT_LITE_CHKNORM_FAIL        ((uint32_t) 0x1)

#define NTT_LITE_REJSAMP_CENTER_EN   ((uint32_t) 0x1)
#define NTT_LITE_REJSAMP_CENTER_DIS  ((uint32_t) 0x0)


int ntt_lite_load_q(uint32_t q, const uint32_t *mu, uint32_t logn, uint32_t logq, uint32_t inv2, uint32_t mode);

int ntt_lite_set_q(uint32_t q);

int ntt_lite_set_ctrl(uint32_t logn, uint32_t k, uint32_t mode);

int ntt_lite_set_inv2(uint32_t inv2);

int ntt_lite_set_mu(const uint32_t *mu, uint32_t mode);

int ntt_lite_set_bound(uint32_t bound);

int ntt_lite_set_clr();

int ntt_lite_set_clr_with_twiddle();

int ntt_lite_load_twiddle(const uint32_t *psi);

int ntt_lite_load_zeta(const uint32_t *zeta);

int ntt_lite_read_twiddle(uint32_t *dst);

int ntt_lite_read_poly(uint32_t *dst);

int ntt_lite_forward_ntt(uint32_t *dst,  const uint32_t *src);

int ntt_lite_backward_ntt(uint32_t *dst,  const uint32_t *src);

int ntt_lite_pwm(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs);

int ntt_lite_mul_const(uint32_t *dst, const uint32_t *lhs);

int ntt_lite_mac(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs);

int ntt_lite_add(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs);

int ntt_lite_add_const(uint32_t *dst, const uint32_t *lhs);

int ntt_lite_sub(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs);

int ntt_lite_sub_const(uint32_t *dst, const uint32_t *lhs);

int ntt_lite_sub_rev(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs);

int ntt_lite_sub_rev_const(uint32_t *dst, const uint32_t *lhs);

int ntt_lite_sum(uint32_t* dst, const uint32_t *src);

int ntt_lite_encode(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_decode(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_cbd(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_rejsamp(uint32_t *dst, const uint32_t *src, uint32_t d, uint32_t center);

int ntt_lite_compress(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_decompress(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_decompress_floor(uint32_t *dst, const uint32_t *src, uint32_t d);

int ntt_lite_decompose(uint32_t *dst_1, uint32_t *dst_0, const uint32_t *src);

int ntt_lite_chknorm(const uint32_t *src);

int ntt_lite_make_hint(uint32_t *dst, const uint32_t *src_0, const uint32_t *src_1);

#endif