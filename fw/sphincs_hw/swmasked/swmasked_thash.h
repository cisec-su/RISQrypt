#ifndef SPX_swmasked_THASH_H
#define SPX_swmasked_THASH_H

#include "swmasked_context.h"
#include "params.h"

#include <stdint.h>

#define swmasked_thash SPX_NAMESPACE(swmasked_thash)
void swmasked_thash(unsigned char *out1, unsigned char *out2,
           const unsigned char *in1, const unsigned char *in2,
           unsigned int inblocks,
           const spx_ctx *ctx, uint32_t addr[8]);

#endif
