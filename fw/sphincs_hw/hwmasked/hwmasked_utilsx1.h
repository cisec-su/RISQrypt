#ifndef SPX_UTILSX4_HWMASKED_H
#define SPX_UTILSX4_HWMASKED_H

#include <stdint.h>
#include "params.h"
#include "swmasked_context.h"

#define treehashx1_hwmasked SPX_NAMESPACE(treehashx1_hwmasked)
void treehashx1_hwmasked(unsigned char *root1, unsigned char *root2,
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
