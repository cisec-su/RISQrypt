#include <stdint.h>

#include "thash.h"
#include "address.h"
#include "params.h"
#include "utils.h"

#include "fips202.h"

/**
 * Takes an array of inblocks concatenated arrays of SPX_N bytes.
 * Uses incremental SHAKE256 to absorb pub_seed and addr directly,
 * avoiding intermediate buffer copies.
 */
void thash(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const spx_ctx *ctx, uint32_t addr[8])
{
    SPX_VLA(uint8_t, bitmask, inblocks * SPX_N);
    unsigned int i;
    uint32_t s_inc[52];

    /* Generate bitmask: SHAKE256(pub_seed || addr) */
    shake256_inc_init(s_inc);
    shake256_inc_absorb(s_inc, ctx->pub_seed, SPX_N);
    shake256_inc_absorb(s_inc, (const uint8_t *)addr, SPX_ADDR_BYTES);
    shake256_inc_finalize(s_inc);
    shake256_inc_squeeze(bitmask, inblocks * SPX_N, s_inc);

    /* Compute output: SHAKE256(pub_seed || addr || (in ^ bitmask)) */
    shake256_inc_init(s_inc);
    shake256_inc_absorb(s_inc, ctx->pub_seed, SPX_N);
    shake256_inc_absorb(s_inc, (const uint8_t *)addr, SPX_ADDR_BYTES);

    /* XOR in with bitmask and absorb in-place */
    for (i = 0; i < inblocks * SPX_N; i++) {
        bitmask[i] = in[i] ^ bitmask[i];
    }
    shake256_inc_absorb(s_inc, bitmask, inblocks * SPX_N);
    shake256_inc_finalize(s_inc);
    shake256_inc_squeeze(out, SPX_N, s_inc);
}
