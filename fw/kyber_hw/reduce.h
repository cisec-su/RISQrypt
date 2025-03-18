#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>


inline void caddq(int16_t *a) {
    *a += (*a >> 15) & KYBER_Q;
}



#endif