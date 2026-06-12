#ifndef masked_SPX_THASH_H
#define masked_SPX_THASH_H

#include "masked_context.h"
#include "params.h"

#include <stdint.h>

#define masked_thash SPX_NAMESPACE(masked_thash)
void masked_thash(unsigned char *out1, unsigned char *out2,
                    const unsigned char *in1, const unsigned char *in2,
                    unsigned int inblocks,
                    const spx_ctx *ctx, uint32_t addr[8]);

#endif
