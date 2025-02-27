#ifndef NTT_LITE_H
#define NTT_LITE_H


#include <stdint.h>


#define NTT_LITE_MODE_SINGLE ((uint32_t) 0x0)
#define NTT_LITE_MODE_DUAL   ((uint32_t) 0x2)
#define NTT_LITE_MODE_POLY   ((uint32_t) 0x3)



int ntt_lite_load_q(const uint32_t *q, const uint32_t *mu, unsigned int logn, unsigned int logq, unsigned int mode);

int ntt_lite_load_twiddle(const uint32_t *psi);

int ntt_lite_forward_ntt(uint32_t *dst,  const uint32_t *src);

int ntt_lite_inverse_ntt(uint32_t *dst,  const uint32_t *src);


#endif