#include "masked_polyvec.h"
#include "ntt_lite.h"
#include "masked_gadgets.h"


static void masked_polyvec_unpack(masked_poly *r, const uint8_t *a, unsigned int len, const uint32_t d, const uint32_t c, const unsigned int step) {
  unsigned int i, j;
  uint32_t rhs = c;
  uint32_t *rhs_ptr;

  for(i = 0; i < len; i++) {
    ntt_lite_decode(r[i].share[0].coeffs, (uint32_t*) a, d);
    a += step;
  }

  for(i = 0; i < len; i++) {
    masked_poly_mask(&r[i], &r[i].share[0]);
  }

  for (j = 0; j < MASKING_N; j++) {
    if (j == 0) {
        rhs = c;
    }
    else {
        rhs = 0;
    }
    for(i = 0; i < len; i++) {
      if (i == 0) {
        rhs_ptr = &rhs;
      }
      else {
        rhs_ptr = NTT_LITE_INPUT_DIS;
      }
      ntt_lite_sub_rev_const(r[i].share[j].coeffs, r[i].share[j].coeffs, rhs_ptr);
    }
  }
}


void masked_polyvecl_eta_unpack(masked_polyvecl *r, const uint8_t *a) {
  masked_polyvec_unpack(r->vec, a, L, LOG_ETA, ETA, POLYETA_PACKEDBYTES);
}


void masked_polyveck_eta_unpack(masked_polyveck *r, const uint8_t *a) {
  masked_polyvec_unpack(r->vec, a, K, LOG_ETA, ETA, POLYETA_PACKEDBYTES);
}


void masked_polyveck_t0_unpack(masked_polyveck *r, const uint8_t *a) {
  masked_polyvec_unpack(r->vec, a, K, D, 1 << (D - 1), POLYT0_PACKEDBYTES);
}


static void masked_polyvec_ntt(masked_poly *r, unsigned int len) {
  unsigned int i;

  for(i = 0; i < len; i++) 
    masked_poly_ntt(&r[i]);
}


void masked_polyvecl_ntt(masked_polyvecl *r) {
  masked_polyvec_ntt(r->vec, L);
}


void masked_polyveck_ntt(masked_polyveck *r) {
  masked_polyvec_ntt(r->vec, K);
}


void masked_polyveck_invntt(masked_poly *r) {
  unsigned int i;

  for(i = 0; i < K; i++) 
    masked_poly_invntt(&r[i]);
}


void masked_polyvecl_uniform_gamma1(masked_polyvecl *y, const masked_crh rhoprime, uint16_t nonce) {
  unsigned int i;
  for (i = 0; i < L; i++) {
    masked_poly_uniform_gamma1(&y->vec[i], rhoprime, L*nonce + i);
  }    
}


void masked_polyvecl_pointwise_acc(masked_poly *w, const polyvecl *u, const masked_polyvecl *v)
{
  unsigned int i;

  masked_poly_pointwise(w, &u->vec[0], &v->vec[0]);
  for(i = 1; i < L; ++i) {
    masked_poly_pointwise_acc(w, &u->vec[i], &v->vec[i]);
  }
}


void masked_polyvec_matrix_pointwise(masked_polyveck *t, const polyvecl mat[K], const masked_polyvecl *v) {
  unsigned int i;

  for(i = 0; i < K; ++i) {
    masked_polyvecl_pointwise_acc(&t->vec[i], &mat[i], v);
  }
}

polyveck temp, temp1;
void masked_polyveck_decompose(polyveck *v1, masked_polyveck *v0, const masked_polyveck *v) {
  unsigned int i, j, t;
  uint32_t mu[2] = {0x02008020, 0x2008};

  for(i = 0; i < K; ++i) {
    for (j = 0; j < MASKING_N; ++j) {
      for (t = 0; t < N; ++t) {
        if (j == 0) {
          temp.vec[i].coeffs[t] = v->vec[i].share[j].coeffs[t];
        }
        else {
          temp.vec[i].coeffs[t] = (temp.vec[i].coeffs[t] + v->vec[i].share[j].coeffs[t]) % Q;
        }
      }
    }
    ntt_lite_set_inv2(GAMMA2_D >> 1);
    ntt_lite_set_mu(mu, NTT_LITE_MODE_SINGLE);
    ntt_lite_set_bound(GAMMA2 << 1);

    poly_decompose(&v1->vec[i], &temp1.vec[i], &temp.vec[i]);
    poly_init_q();
    masked_poly_mask(&v0->vec[i], &temp1.vec[i]);
  }

}


int masked_polyvecl_pointwise_add_invntt_chknorm(masked_polyvecl *r, const masked_polyvecl *v, const poly *c, const masked_polyvecl *u, uint32_t B) {
  unsigned int i;
  for (i = 0; i < L; i++) {
    if (masked_poly_pointwise_add_invntt_chknorm(&r->vec[i], &v->vec[i], c, &u->vec[i], B)) {
      return 1;
    }
  }
  return 0;
}


