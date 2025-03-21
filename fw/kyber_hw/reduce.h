#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>
#include "params.h"


inline void caddq(int16_t *a) {
    *a += (*a >> 15) & KYBER_Q;
}


inline void csubq(int16_t *a) {
    *a -= KYBER_Q;
    *a += (*a >> 15) & KYBER_Q;
}


#endif