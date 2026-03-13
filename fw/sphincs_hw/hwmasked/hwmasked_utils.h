#ifndef SPX_HWMASKED_UTILS_H
#define SPX_HWMASKED_UTILS_H

#include <stdint.h>
#include "params.h"
#include "swmasked_context.h"

/* To support MSVC use alloca() instead of VLAs. See #20. */
#ifdef _MSC_VER
# include <malloc.h>
# define SPX_VLA(__t,__x,__s) __t *__x = (__t*)_alloca((__s)*sizeof(__t))
#else
# define SPX_VLA(__t,__x,__s) __t __x[__s]
#endif

#define ull_to_bytes SPX_NAMESPACE(ull_to_bytes)
void ull_to_bytes(unsigned char *out, unsigned int outlen,
                  unsigned long long in);
#define u32_to_bytes SPX_NAMESPACE(u32_to_bytes)
void u32_to_bytes(unsigned char *out, uint32_t in);

#define bytes_to_ull SPX_NAMESPACE(bytes_to_ull)
unsigned long long bytes_to_ull(const unsigned char *in, unsigned int inlen);

#define compute_root_hwmasked SPX_NAMESPACE(compute_root_hwmasked)
void compute_root_hwmasked(unsigned char *root1, unsigned char *root2,
                           const unsigned char *leaf1, const unsigned char *leaf2,
                           uint32_t leaf_idx, uint32_t idx_offset,
                           const unsigned char *auth_path, uint32_t tree_height,
                           const spx_ctx *ctx, uint32_t addr[8]);

#define treehash_hwmasked SPX_NAMESPACE(treehash_hwmasked)
void treehash_hwmasked(unsigned char *root1, unsigned char *root2,
                       unsigned char *auth_path,
                       const spx_ctx* ctx,
                       uint32_t leaf_idx, uint32_t idx_offset,
                       uint32_t tree_height,
                       void (*gen_leaf_hwmasked)(
                           unsigned char*,
                           unsigned char* ,
                           const spx_ctx*,
                           uint32_t idx, void *info),
                       uint32_t tree_addr[8],
                       void *info);

#endif
