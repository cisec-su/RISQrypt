#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "util.h"

#include "hwmasked_fors.h"
#include "hwmasked_utils.h"
#include "hwmasked_utilsx1.h"
#include "hwmasked_hash.h"
#include "hwmasked_thash.h"
#include "address.h"
#include "randombytes.h"

struct fors_gen_leaf_info_hw {
    uint32_t leaf_addrx[8];
};

static void message_to_indices_hw(uint32_t *indices, const unsigned char *m)
{
    unsigned int i, j;
    unsigned int offset = 0;

    for (i = 0; i < SPX_FORS_TREES; i++) {
        indices[i] = 0;
        for (j = 0; j < SPX_FORS_HEIGHT; j++) {
            indices[i] ^= ((m[offset >> 3] >> (offset & 0x7)) & 1u) << j;
            offset++;
        }
    }
}

static void fors_gen_sk_hwmasked(unsigned char *sk1, unsigned char *sk2,
                                 const spx_ctx *ctx,
                                 uint32_t fors_leaf_addr[8])
{
    hwmasked_prf_addr(sk1, sk2, ctx, fors_leaf_addr);
}

static void fors_sk_to_leaf_hwmasked(unsigned char *leaf1, unsigned char *leaf2,
                                     const unsigned char *sk1, const unsigned char *sk2,
                                     const spx_ctx *ctx,
                                     uint32_t fors_leaf_addr[8])
{
    hwmasked_thash(leaf1, leaf2, sk1, sk2, 1, ctx, fors_leaf_addr);
}

static void fors_gen_leafx1_hwmasked(unsigned char *leaf1, unsigned char *leaf2,
                                     const spx_ctx *ctx,
                                     uint32_t addr_idx, void *info)
{
    struct fors_gen_leaf_info_hw *fors_info = info;
    uint32_t *fors_leaf_addr = fors_info->leaf_addrx;

    set_tree_index(fors_leaf_addr, addr_idx);
    set_tree_height(fors_leaf_addr, 0);
    set_type(fors_leaf_addr, SPX_ADDR_TYPE_FORSPRF);

    fors_gen_sk_hwmasked(leaf1, leaf2, ctx, fors_leaf_addr);

    set_type(fors_leaf_addr, SPX_ADDR_TYPE_FORSTREE);
    fors_sk_to_leaf_hwmasked(leaf1, leaf2,
                             leaf1, leaf2,
                             ctx, fors_leaf_addr);
}

// HW masked version of fors_sign
void fors_sign_hwmasked(unsigned char *sig, unsigned char *pk,
                        const unsigned char *m,
                        const spx_ctx *ctx,
                        const uint32_t fors_addr[8])
{
    uint32_t indices[SPX_FORS_TREES];

    SPX_VLA(uint8_t, roots1, SPX_FORS_TREES * SPX_N);
    SPX_VLA(uint8_t, roots2, SPX_FORS_TREES * SPX_N);

    unsigned char sk_share1[SPX_N];
    unsigned char sk_share2[SPX_N];

    uint32_t fors_tree_addr[8] = {0};
    struct fors_gen_leaf_info_hw fors_info = {0};
    uint32_t *fors_leaf_addr = fors_info.leaf_addrx;
    uint32_t fors_pk_addr[8] = {0};
    uint32_t idx_offset;
    unsigned int i;

    copy_keypair_addr(fors_tree_addr, fors_addr);
    copy_keypair_addr(fors_leaf_addr, fors_addr);

    copy_keypair_addr(fors_pk_addr, fors_addr);
    set_type(fors_pk_addr, SPX_ADDR_TYPE_FORSPK);

    message_to_indices_hw(indices, m);

    for (i = 0; i < SPX_FORS_TREES; i++) {
        idx_offset = i * (1 << SPX_FORS_HEIGHT);
        set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSPRF);

        set_tree_height(fors_tree_addr, 0);
        set_tree_index(fors_tree_addr, indices[i] + idx_offset);
        set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSPRF);

        fors_gen_sk_hwmasked(sk_share1, sk_share2, ctx, fors_tree_addr);

        set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSTREE);

        for(int j=0; j<SPX_N; j++) {
            sig[j] = sk_share1[j] ^ sk_share2[j];
        }
        sig += SPX_N;

        treehashx1_hwmasked(roots1 + i*SPX_N, roots2 + i*SPX_N,
                            sig,
                            ctx,
                            indices[i], idx_offset, SPX_FORS_HEIGHT,
                            fors_gen_leafx1_hwmasked,
                            fors_tree_addr, &fors_info);

        sig += SPX_N * SPX_FORS_HEIGHT;
    }

    unsigned char pk1[SPX_N];
    unsigned char pk2[SPX_N];

    hwmasked_thash(pk1, pk2,
                   roots1, roots2,
                   SPX_FORS_TREES, ctx, fors_pk_addr);

    for(int j=0; j<SPX_N; j++) {
        pk[j] = pk1[j] ^ pk2[j];
    }
}

// HW masked version of fors_pk_from_sig
void fors_pk_from_sig_hwmasked(unsigned char *pk,
                               const unsigned char *sig, const unsigned char *m,
                               const spx_ctx* ctx,
                               const uint32_t fors_addr[8])
{
    uint32_t indices[SPX_FORS_TREES];

    SPX_VLA(uint8_t, roots1, SPX_FORS_TREES * SPX_N);
    SPX_VLA(uint8_t, roots2, SPX_FORS_TREES * SPX_N);

    unsigned char leaf1[SPX_N];
    unsigned char leaf2[SPX_N];
    unsigned char rand_mask[SPX_N];

    uint32_t fors_tree_addr[8] = {0};
    uint32_t fors_pk_addr[8] = {0};
    uint32_t idx_offset;
    unsigned int i;

    copy_keypair_addr(fors_tree_addr, fors_addr);
    copy_keypair_addr(fors_pk_addr, fors_addr);

    set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSTREE);
    set_type(fors_pk_addr, SPX_ADDR_TYPE_FORSPK);

    message_to_indices_hw(indices, m);

    for (i = 0; i < SPX_FORS_TREES; i++) {
        idx_offset = i * (1 << SPX_FORS_HEIGHT);

        set_tree_height(fors_tree_addr, 0);
        set_tree_index(fors_tree_addr, indices[i] + idx_offset);

        randombytes(rand_mask, SPX_N);
        unsigned char sk_in1[SPX_N];
        unsigned char sk_in2[SPX_N];

        for(int j=0; j<SPX_N; j++) {
            sk_in1[j] = sig[j] ^ rand_mask[j];
            sk_in2[j] = rand_mask[j];
        }

        fors_sk_to_leaf_hwmasked(leaf1, leaf2, sk_in1, sk_in2, ctx, fors_tree_addr);
        sig += SPX_N;

        compute_root_hwmasked(roots1 + i*SPX_N, roots2 + i*SPX_N,
                              leaf1, leaf2,
                              indices[i], idx_offset,
                              sig, SPX_FORS_HEIGHT, ctx, fors_tree_addr);

        sig += SPX_N * SPX_FORS_HEIGHT;
    }

    unsigned char pk1[SPX_N];
    unsigned char pk2[SPX_N];

    hwmasked_thash(pk1, pk2, roots1, roots2, SPX_FORS_TREES, ctx, fors_pk_addr);

    for(int j=0; j<SPX_N; j++) {
        pk[j] = pk1[j] ^ pk2[j];
    }
}
