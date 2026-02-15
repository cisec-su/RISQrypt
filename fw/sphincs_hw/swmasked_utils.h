#ifndef SPX_swmasked_utils_H
#define SPX_swmasked_utils_H

#include <stdint.h>
#include "params.h"
#include "swmasked_context.h"


/* To support MSVC use alloca() instead of VLAs. See #20. */
#ifdef _MSC_VER
/* MSVC defines _alloca in malloc.h */
# include <malloc.h>
/* Note: _malloca(), which is recommended over deprecated _alloca,
   requires that you call _freea(). So we stick with _alloca */ 
# define SPX_VLA(__t,__x,__s) __t *__x = (__t*)_alloca((__s)*sizeof(__t))
#else
# define SPX_VLA(__t,__x,__s) __t __x[__s]
#endif

/**
 * Converts the value of 'in' to 'outlen' bytes in big-endian byte order.
 */
#define ull_to_bytes SPX_NAMESPACE(ull_to_bytes)
void ull_to_bytes(unsigned char *out, unsigned int outlen,
                  unsigned long long in);
#define u32_to_bytes SPX_NAMESPACE(u32_to_bytes)
void u32_to_bytes(unsigned char *out, uint32_t in);

/**
 * Converts the inlen bytes in 'in' from big-endian byte order to an integer.
 */
#define bytes_to_ull SPX_NAMESPACE(bytes_to_ull)
unsigned long long bytes_to_ull(const unsigned char *in, unsigned int inlen);


#define compute_root_masked SPX_NAMESPACE(compute_root_masked)
void compute_root_masked(unsigned char *root1, unsigned char *root2,
                         const unsigned char *leaf1, const unsigned char *leaf2,
                         uint32_t leaf_idx, uint32_t idx_offset,
                         const unsigned char *auth_path, uint32_t tree_height,
                         const spx_ctx *ctx, uint32_t addr[8]);


#define treehash_masked SPX_NAMESPACE(treehash_masked)
void treehash_masked(unsigned char *root1, unsigned char *root2,
                     unsigned char *auth_path,
                     const spx_ctx* ctx,
                     uint32_t leaf_idx, uint32_t idx_offset,
                     uint32_t tree_height,
                     void (*gen_leaf_masked)(
                         unsigned char*,
                         unsigned char* ,
                         const spx_ctx*,
                         uint32_t idx, void *info),
                     uint32_t tree_addr[8],
                     void *info);

#endif
