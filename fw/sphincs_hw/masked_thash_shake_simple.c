#include <stdint.h>
#include <string.h>

#include "masked_thash.h"
#include "address.h"
#include "params.h"
#include "masked_utils.h"
#include "masked_fips202.h"

void masked_thash(unsigned char *out1, unsigned char *out2,
                const unsigned char *in1, const unsigned char *in2,
                unsigned int inblocks,
                const spx_ctx *ctx, uint32_t addr[8])
{
    SPX_VLA(uint8_t, buf1, SPX_N + SPX_ADDR_BYTES + inblocks*SPX_N);
    SPX_VLA(uint8_t, buf2, SPX_N + SPX_ADDR_BYTES + inblocks*SPX_N);

    memcpy(buf1, ctx->pub_seed, SPX_N);
    memcpy(buf1 + SPX_N, addr, SPX_ADDR_BYTES);
    memcpy(buf1 + SPX_N + SPX_ADDR_BYTES, in1, inblocks * SPX_N);

    memset(buf2, 0, SPX_N + SPX_ADDR_BYTES);
    memcpy(buf2 + SPX_N + SPX_ADDR_BYTES, in2, inblocks * SPX_N);

    masked_shake256(out1, out2, SPX_N, buf1, buf2, SPX_N + SPX_ADDR_BYTES + inblocks*SPX_N);
}
