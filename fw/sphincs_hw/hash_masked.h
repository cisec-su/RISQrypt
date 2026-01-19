#ifndef SPX_HASH_MASKED_H
#define SPX_HASH_MASKED_H

#include <stdint.h>
#include "context_masked.h"
#include "params.h"

#define initialize_hash_function SPX_NAMESPACE(initialize_hash_function)
void initialize_hash_function(spx_ctx *ctx);

#define masked_prf_addr SPX_NAMESPACE(masked_prf_addr)
void masked_prf_addr(unsigned char *out1, unsigned char *out2, 
                    const spx_ctx *ctx,
                    const uint32_t addr[8]);
                    
#define gen_message_random SPX_NAMESPACE(gen_message_random)
void gen_message_random(unsigned char *R, const unsigned char *sk_prf,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen,
                        const spx_ctx *ctx);

#define hash_message SPX_NAMESPACE(hash_message)
void hash_message(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen,
                  const spx_ctx *ctx);

#endif
