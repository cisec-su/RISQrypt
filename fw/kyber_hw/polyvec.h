#ifndef POLYVEC_H
#define POLYVEC_H


#include <stdint.h>
#include "params.h"
#include "poly.h"


typedef struct{
    poly vec[KYBER_K];
} polyvec;


#define polyvec_compress KYBER_NAMESPACE(_polyvec_compress)
void polyvec_compress(uint8_t r[KYBER_POLYVECCOMPRESSEDBYTES], polyvec *a);
#define polyvec_decompress KYBER_NAMESPACE(_polyvec_decompress)
void polyvec_decompress(polyvec *r, const uint8_t a[KYBER_POLYVECCOMPRESSEDBYTES]);
#define polyvec_decompress_compress KYBER_NAMESPACE(_polyvec_decompress_compress)
void polyvec_decompress_compress(polyvec *r, const uint8_t a[KYBER_POLYVECCOMPRESSEDBYTES]);

#define polyvec_tobytes KYBER_NAMESPACE(_polyvec_tobytes)
void polyvec_tobytes(uint8_t r[KYBER_POLYVECBYTES], polyvec *a);
#define polyvec_frombytes KYBER_NAMESPACE(_polyvec_frombytes)
void polyvec_frombytes(polyvec *r, const uint8_t a[KYBER_POLYVECBYTES]);

#define polyvec_ntt KYBER_NAMESPACE(_polyvec_ntt)
void polyvec_ntt(polyvec *r);

#define polyvec_pointwise_acc_invntt \
        KYBER_NAMESPACE(_polyvec_pointwise_acc_invntt)
void polyvec_pointwise_acc_invntt(poly *r,
                                  const polyvec *a,
                                  const polyvec *b);

#define polyvec_pointwise_acc \
        KYBER_NAMESPACE(_polyvec_pointwise_acc)
void polyvec_pointwise_acc(poly *r,
                           const polyvec *a,
                           const polyvec *b);

#define polyvec_add KYBER_NAMESPACE(_polyvec_add)
void polyvec_add(polyvec *r, const polyvec *a, const polyvec *b);



#endif
