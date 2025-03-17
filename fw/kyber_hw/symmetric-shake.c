#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "keccak.h"
#include "symmetric.h"




void kyber_shake128_init() {
  keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_DIS);
}


/*************************************************
* Name:        kyber_shake128_absorb
*
* Description: Absorb step of the SHAKE128 specialized for the Kyber context.
*
* Arguments:   - keccak_state *state: pointer to (uninitialized) output
*                                     Keccak state
*              - const uint8_t *seed: pointer to KYBER_SYMBYTES input
*                                     to be absorbed into state
*              - uint8_t i            additional byte of input
*              - uint8_t j            additional byte of input
**************************************************/
void kyber_shake128_absorb(const uint8_t seed[KYBER_SYMBYTES],
                           uint8_t x,
                           uint8_t y) {
  volatile uint32_t t;
  keccak_absorb(seed, NULL, KYBER_SYMBYTES >> 2);
  t = (0x1F << 16) | (((uint32_t) y) << 8) | ((uint32_t) x);
  keccak_finish(&t);
}



void kyber_shake128_squeezeblocks(uint8_t *out,
                                  unsigned int num_blocks) {
  keccak_squeeze(out, NULL, num_blocks*(SHAKE128_RATE >> 2));
}


/*************************************************
* Name:        kyber_shake256_prf
*
* Description: Usage of SHAKE256 as a PRF, concatenates secret and public input
*              and then generates outlen bytes of SHAKE256 output
*
* Arguments:   - uint8_t *out:       pointer to output
*              - size_t outlen:      number of requested output bytes
*              - const uint8_t *key: pointer to the key
*                                    (of length KYBER_SYMBYTES)
*              - uint8_t nonce:      single-byte nonce (public PRF input)
**************************************************/
void kyber_shake256_prf(uint8_t *out,
                        size_t outlen,
                        const uint8_t key[KYBER_SYMBYTES],
                        uint8_t nonce)
{
  volatile uint32_t t;
  keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
  keccak_absorb(key, NULL, KYBER_SYMBYTES >> 2);
  t = (0x1F << 8) | ((uint32_t) nonce);
  keccak_finish(&t);
  keccak_squeeze(out, NULL, outlen >> 2);
}
