#ifndef SPX_HASH_swmasked_H
#define SPX_HASH_swmasked_H

#include <stdint.h>
#include "swmasked_context.h"
#include "params.h"

#define initialize_hash_function_masked SPX_NAMESPACE(initialize_hash_function_masked)
void initialize_hash_function_masked(spx_ctx *ctx);

#define swmasked_prf_addr SPX_NAMESPACE(swmasked_prf_addr)
void swmasked_prf_addr(unsigned char *out1, unsigned char *out2, 
                    const spx_ctx *ctx,
                    const uint32_t addr[8]);
                    
#define gen_message_random_masked SPX_NAMESPACE(gen_message_random_masked)
void gen_message_random_masked(unsigned char *R, const unsigned char *sk_prf,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen,
                        const spx_ctx *ctx);

#define hash_message_masked SPX_NAMESPACE(hash_message_masked)
void hash_message_masked(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen,
                  const spx_ctx *ctx);

#endif
