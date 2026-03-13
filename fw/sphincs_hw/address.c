#include <stdint.h>

#include "address.h"
#include "params.h"
#include "utils.h"

/*
 * Specify which level of Merkle tree (the "layer") we're working on
 */
void set_layer_addr(uint32_t addr[8], uint32_t layer)
{
    ((unsigned char *)addr)[SPX_OFFSET_LAYER] = (unsigned char)layer;
}

/*
 * Specify which Merkle tree within the level (the "tree address") we're working on
 */
void set_tree_addr(uint32_t addr[8], const uint32_t tree[2])
{
#if (SPX_TREE_HEIGHT * (SPX_D - 1)) > 64
    #error Subtree addressing is currently limited to at most 2^64 trees
#endif
    // Big-endian: High word first, then Low word
    u32_to_bytes(&((unsigned char *)addr)[SPX_OFFSET_TREE], tree[1]);
    u32_to_bytes(&((unsigned char *)addr)[SPX_OFFSET_TREE + 4], tree[0]);
}

/*
 * Specify the reason we'll use this address structure for, that is, what
 * hash will we compute with it.  This is used so that unrelated types of
 * hashes don't accidentally get the same address structure.  The type will be
 * one of the SPX_ADDR_TYPE constants
 */
void set_type(uint32_t addr[8], uint32_t type)
{
    ((unsigned char *)addr)[SPX_OFFSET_TYPE] = (unsigned char)type;
}

/*
 * Copy the layer and tree fields of the address structure.  This is used
 * when we're doing multiple types of hashes within the same Merkle tree
 * SPX_OFFSET_TREE + 8 = 16 bytes = 4 uint32_t words
 */
void copy_subtree_addr(uint32_t out[8], const uint32_t in[8])
{
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
}

/* These functions are used for OTS addresses. */

/*
 * Specify which Merkle leaf we're working on; that is, which OTS keypair
 * we're talking about.
 */
void set_keypair_addr(uint32_t addr[8], uint32_t keypair)
{
    u32_to_bytes(&((unsigned char *)addr)[SPX_OFFSET_KP_ADDR], keypair);
}

/*
 * Copy the layer, tree and keypair fields of the address structure.  This is
 * used when we're doing multiple things within the same OTS keypair
 * SPX_OFFSET_KP_ADDR = 20 sits within word index 5
 */
void copy_keypair_addr(uint32_t out[8], const uint32_t in[8])
{
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
    out[5] = in[5];
}

/*
 * Specify which Merkle chain within the OTS we're working with
 * (the chain address)
 */
void set_chain_addr(uint32_t addr[8], uint32_t chain)
{
    ((unsigned char *)addr)[SPX_OFFSET_CHAIN_ADDR] = (unsigned char)chain;
}

/*
 * Specify where in the Merkle chain we are
* (the hash address)
 */
void set_hash_addr(uint32_t addr[8], uint32_t hash)
{
    ((unsigned char *)addr)[SPX_OFFSET_HASH_ADDR] = (unsigned char)hash;
}

/* These functions are used for all hash tree addresses (including FORS). */

/*
 * Specify the height of the node in the Merkle/FORS tree we are in
 * (the tree height)
 */
void set_tree_height(uint32_t addr[8], uint32_t tree_height)
{
    ((unsigned char *)addr)[SPX_OFFSET_TREE_HGT] = (unsigned char)tree_height;
}

/*
 * Specify the distance from the left edge of the node in the Merkle/FORS tree
 * (the tree index)
 */
void set_tree_index(uint32_t addr[8], uint32_t tree_index)
{
    u32_to_bytes(&((unsigned char *)addr)[SPX_OFFSET_TREE_INDEX], tree_index );
}

