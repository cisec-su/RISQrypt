#include <stdint.h>
#include <string.h>

#include "swmasked_thash.h"
#include "address.h"
#include "params.h"
#include "swmasked_utils.h"
#include "swmasked_fips202.h"

void swmasked_thash(unsigned char *out1, unsigned char *out2,
                const unsigned char *in1, const unsigned char *in2,
                unsigned int inblocks,
                const spx_ctx *ctx, uint32_t addr[8])
{
    (void)ctx;
    (void)addr;
    
    swmasked_shake256(out1, out2, SPX_N, in1, in2, inblocks * SPX_N);
}
