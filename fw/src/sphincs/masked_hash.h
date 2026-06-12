#ifndef SPX_HASH_HWMASKED_H
#define SPX_HASH_HWMASKED_H

#include <stdint.h>
#include "masked_context.h"
#include "params.h"

#define initialize_hash_function_hwmasked SPX_NAMESPACE(initialize_hash_function_hwmasked)
void initialize_hash_function_hwmasked(spx_ctx *ctx);

#define hwmasked_prf_addr SPX_NAMESPACE(hwmasked_prf_addr)
void hwmasked_prf_addr(unsigned char *out1, unsigned char *out2,
                       const spx_ctx *ctx,
                       const uint32_t addr[8]);

#define gen_message_random_hwmasked SPX_NAMESPACE(gen_message_random_hwmasked)
void gen_message_random_hwmasked(unsigned char *R, const unsigned char *sk_prf,
                                 const unsigned char *optrand,
                                 const unsigned char *m, unsigned long long mlen,
                                 const spx_ctx *ctx);

#define hash_message_hwmasked SPX_NAMESPACE(hash_message_hwmasked)
void hash_message_hwmasked(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                           const unsigned char *R, const unsigned char *pk,
                           const unsigned char *m, unsigned long long mlen,
                           const spx_ctx *ctx);

#endif
