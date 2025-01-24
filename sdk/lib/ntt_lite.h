#ifndef NTT_LITE_H
#define NTT_LITE_H


#include <stdint.h>


int ntt_lite_forward(uint32_t *b,  uint32_t *a, uint32_t *w, uint32_t *q, unsigned int logn, unsigned int logq);


int ntt_lite_forward_2d(uint32_t *b,  uint32_t *a, uint32_t *w0, uint32_t *w1, uint32_t *q, unsigned int logn, unsigned int logn0, unsigned int logq);


#endif