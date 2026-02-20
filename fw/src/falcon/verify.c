#include "poly.h"
#include "params.h"
#include "verify.h"



int verify_raw(const poly *c0, const poly *s2, poly *h) {
    poly temp;
    poly_init_ntt();
    poly_ntt_from_center(&temp, s2);
    poly_ntt(h);
    poly_basemul(&temp, &temp, h);
    poly_init_invntt();
    poly_invntt_sub_to_center(&temp, c0);
    return poly_is_short(&temp, s2);
}