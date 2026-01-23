#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"


typedef struct{
    int16_t coeffs[KYBER_N];
} poly;


typedef struct{
    uint32_t coeffs[KYBER_N];
} poly_u32;


#define poly_compress KYBER_NAMESPACE(_poly_compress)
void poly_compress(uint8_t r[KYBER_POLYCOMPRESSEDBYTES], poly *a);

#define poly_decompress KYBER_NAMESPACE(_poly_decompress)
void poly_decompress(poly *r, const uint8_t a[KYBER_POLYCOMPRESSEDBYTES]);

#define poly_tobytes KYBER_NAMESPACE(_poly_tobytes)
void poly_tobytes(uint8_t r[KYBER_POLYBYTES], poly *a);

#define poly_tobytes_fromhw KYBER_NAMESPACE(_poly_tobytes_fromhw)
void poly_tobytes_fromhw(uint8_t r[KYBER_POLYBYTES]);

#define poly_frombytes KYBER_NAMESPACE(_poly_frombytes)
void poly_frombytes(poly *r, const uint8_t a[KYBER_POLYBYTES]);

#define poly_frommsg KYBER_NAMESPACE(_poly_frommsg)
void poly_frommsg(poly *r, const uint8_t msg[KYBER_INDCPA_MSGBYTES]);

#define poly_tomsg KYBER_NAMESPACE(_poly_tomsg)
void poly_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], poly *r);

#define poly_getnoise_eta1_fromhw KYBER_NAMESPACE(_poly_getnoise_eta1_fromhw)
void poly_getnoise_eta1_fromhw(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce, int absorb_next);

#define poly_getnoise_eta2_fromhw KYBER_NAMESPACE(_poly_getnoise_eta2_fromhw)
void poly_getnoise_eta2_fromhw(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce, int absorb_next);

#define poly_ntt KYBER_NAMESPACE(_poly_ntt)
void poly_ntt(poly *r);

#define poly_invntt KYBER_NAMESPACE(_poly_invntt)
void poly_invntt(poly *r);

#define poly_init_zeta KYBER_NAMESPACE(_poly_init_zeta)
void poly_init_zeta();

#define poly_basemul KYBER_NAMESPACE(_poly_basemul)
void poly_basemul(poly *r, const poly *a, const poly *b);

#define poly_add KYBER_NAMESPACE(_poly_add)
void poly_add(poly *r, const poly *a, const poly *b);

#define poly_add_chain KYBER_NAMESPACE(_poly_add_chain)
void poly_add_chain(poly *r, const poly *a, const poly *b, const poly *c);

#define poly_sub KYBER_NAMESPACE(_poly_sub)
void poly_sub(poly *r, const poly *a, const poly *b);

#define poly_sub_tomsg KYBER_NAMESPACE(_poly_sub_tomsg)
void poly_sub_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *a, const poly *b);

#define poly_sub_tomsg_fromhw KYBER_NAMESPACE(_poly_sub_tomsg_fromhw)
void poly_sub_tomsg_fromhw(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *a);

#define poly_init_q KYBER_NAMESPACE(_poly_init_q)
void poly_init_q();

#define poly_set_inv2 KYBER_NAMESPACE(_poly_set_inv2)
void poly_set_inv2();

#define poly_set_ctrl KYBER_NAMESPACE(_poly_set_ctrl)
void poly_set_ctrl();

#define poly_init_ntt KYBER_NAMESPACE(_poly_init_ntt)
void poly_init_ntt();

#define poly_init_invntt KYBER_NAMESPACE(_poly_init_invntt)
void poly_init_invntt();

#define poly_add_chain_pack_dv_fromhw KYBER_NAMESPACE(_poly_add_chain_pack_dv_fromhw)
void poly_add_chain_pack_dv_fromhw(uint8_t r[KYBER_POLYCOMPRESSEDBYTES], const poly *b, const poly *c);

#define poly_add_pack_du_fromhw KYBER_NAMESPACE(_poly_add_pack_du_fromhw)
void poly_add_pack_du_fromhw(uint8_t *r, const poly *b);

#endif
