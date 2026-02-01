#include <stdint.h>
#include "params.h"


void caddq(int16_t *a) {

#ifdef KYBER_Q
    if (*a < 0) {
        *a += KYBER_Q;
    }
#else
    if (*a < 0) {
        *a += Q;
    }
#endif
}
