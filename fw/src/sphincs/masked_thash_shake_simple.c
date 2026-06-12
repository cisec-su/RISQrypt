#include <stdint.h>
#include <string.h>

#include "masked_thash.h"
#include "address.h"
#include "params.h"
#include "masked_utils.h"
#include "masked_fips202.h"

/**
 * HW-accelerated masked thash using HW masked SHAKE256.
 * Takes an array of inblocks concatenated arrays of SPX_N bytes (two shares).
 */
void masked_thash(unsigned char *out1, unsigned char *out2,
                    const unsigned char *in1, const unsigned char *in2,
                    unsigned int inblocks,
                    const spx_ctx *ctx, uint32_t addr[8])
{
    (void)ctx;
    (void)addr;

    masked_shake256(out1, out2, SPX_N, in1, in2, inblocks * SPX_N);
}
