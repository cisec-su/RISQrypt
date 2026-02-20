#ifndef PACK_H
#define PACK_H

#include <stdint.h>
#include "params.h"
#include "poly.h"
#include "polyvec.h"



void pack_pk(uint8_t r[KYBER_INDCPA_PUBLICKEYBYTES], polyvec *pk, const uint8_t seed[KYBER_SYMBYTES]);

void unpack_pk(polyvec *pk, uint8_t seed[KYBER_SYMBYTES], const uint8_t packedpk[KYBER_INDCPA_PUBLICKEYBYTES]);

void pack_sk(uint8_t r[KYBER_INDCPA_SECRETKEYBYTES], polyvec *sk);

void unpack_sk(polyvec *sk, uint8_t const packedsk[KYBER_INDCPA_SECRETKEYBYTES]);

void pack_ciphertext(uint8_t r[KYBER_INDCPA_BYTES], polyvec *b, poly *v);

void unpack_ciphertext(polyvec *b, poly *v, const uint8_t c[KYBER_INDCPA_BYTES]);

void absorb_routine(const uint8_t seed[KYBER_SYMBYTES], int i, int j, int transposed);

#define gen_matrix KYBER_NAMESPACE(_gen_matrix)
void gen_matrix(polyvec *a, const uint8_t seed[KYBER_SYMBYTES], int transposed);

#define gen_poly_tohw KYBER_NAMESPACE(_gen_poly_tohw)
void gen_poly_tohw(const uint8_t seed[KYBER_SYMBYTES], unsigned int i, unsigned int j, int transposed, int absorb_next);

#define gen_a(A,B)  gen_matrix(A,B,0)
#define gen_at(A,B) gen_matrix(A,B,1)



#endif