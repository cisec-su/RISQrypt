#ifndef MASKED_SYMMETRIC_H
#define MASKED_SYMMETRIC_H

#include "symmetric.h"
#include "masked.h"


#define kyber_masked_shake256_prf KYBER_NAMESPACE(_kyber_masked_shake256_prf)
void kyber_masked_shake256_prf(masked_ptr dst, size_t dst_len, const masked_sym src, uint8_t nonce);

void masked_shake256(masked_ptr dst, size_t dst_len, const masked_ptr src, size_t src_len);

void masked_sha3_512(masked_ptr dst, const masked_ptr src, size_t src_len);



#define masked_prf(OUT, OUTBYTES, KEY, NONCE) kyber_masked_shake256_prf(OUT, OUTBYTES, KEY, NONCE)

#endif /* MASKED_SYMMETRIC_H */
