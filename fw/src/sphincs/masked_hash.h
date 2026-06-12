#ifndef masked_SPX_HASH_H
#define masked_SPX_HASH_H

#include <stdint.h>
#include "masked_context.h"
#include "params.h"

#define masked_initialize_hash_function SPX_NAMESPACE(masked_initialize_hash_function)
void masked_initialize_hash_function(spx_ctx *ctx);

#define masked_prf_addr SPX_NAMESPACE(masked_prf_addr)
void masked_prf_addr(unsigned char *out1, unsigned char *out2,
                       const spx_ctx *ctx,
                       const uint32_t addr[8]);

#define masked_gen_message_random SPX_NAMESPACE(masked_gen_message_random)
void masked_gen_message_random(unsigned char *R, const unsigned char *sk_prf,
                                 const unsigned char *optrand,
                                 const unsigned char *m, unsigned long long mlen,
                                 const spx_ctx *ctx);

#define masked_hash_message SPX_NAMESPACE(masked_hash_message)
void masked_hash_message(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                           const unsigned char *R, const unsigned char *pk,
                           const unsigned char *m, unsigned long long mlen,
                           const spx_ctx *ctx);

#endif
