#ifndef SPX_HWMASKED_THASH_H
#define SPX_HWMASKED_THASH_H

#include "swmasked_context.h"
#include "params.h"

#include <stdint.h>

#define hwmasked_thash SPX_NAMESPACE(hwmasked_thash)
void hwmasked_thash(unsigned char *out1, unsigned char *out2,
                    const unsigned char *in1, const unsigned char *in2,
                    unsigned int inblocks,
                    const spx_ctx *ctx, uint32_t addr[8]);

#endif
