#include <string.h>

#include "utils_masked.h"
#include "params.h"
#include "hash_masked.h"
#include "thash_masked.h"
#include "address.h"
#include "randombytes.h"
#include "wotsx1_masked.h" 


// Non-masked functions from original file
/**
 * Converts the value of 'in' to 'outlen' bytes in big-endian byte order.
 */
void ull_to_bytes(unsigned char *out, unsigned int outlen,
                  unsigned long long in)
{
    int i;

    /* Iterate over out in decreasing order, for big-endianness. */
    for (i = (signed int)outlen - 1; i >= 0; i--) {
        out[i] = in & 0xff;
        in = in >> 8;
    }
}

void u32_to_bytes(unsigned char *out, uint32_t in)
{
    out[0] = (unsigned char)(in >> 24);
    out[1] = (unsigned char)(in >> 16);
    out[2] = (unsigned char)(in >> 8);
    out[3] = (unsigned char)in;
}

/**
 * Converts the inlen bytes in 'in' from big-endian byte order to an integer.
 */
unsigned long long bytes_to_ull(const unsigned char *in, unsigned int inlen)
{
    unsigned long long retval = 0;
    unsigned int i;

    for (i = 0; i < inlen; i++) {
        retval |= ((unsigned long long)in[i]) << (8*(inlen - 1 - i));
    }
    return retval;
}

// Masked version of compute_root
void compute_root_masked(unsigned char *root1, unsigned char *root2,
                         const unsigned char *leaf1, const unsigned char *leaf2,
                         uint32_t leaf_idx, uint32_t idx_offset,
                         const unsigned char *auth_path, uint32_t tree_height,
                         const spx_ctx *ctx, uint32_t addr[8])
{
    uint32_t i;
    unsigned char buffer1[2 * SPX_N];
    unsigned char buffer2[2 * SPX_N];

    // Temporary buffer to hold the random mask for the auth path
    unsigned char auth_mask[SPX_N];

    
    if (leaf_idx & 1) { // If leaf_idx odd -> right child
        memcpy(buffer1 + SPX_N, leaf1, SPX_N);
        memcpy(buffer2 + SPX_N, leaf2, SPX_N);
        
        // Randomly mask the public auth_path element into the left half
        randombytes(auth_mask, SPX_N);
        for (int j=0; j<SPX_N; j++) {
            buffer1[j] = auth_path[j] ^ auth_mask[j]; // Share1 = Path ^ R
            buffer2[j] = auth_mask[j];               // Share2 = R
        }
    }
    else {
        // Copy secret leaf shares to the left half
        memcpy(buffer1, leaf1, SPX_N);
        memcpy(buffer2, leaf2, SPX_N);

        // Randomly mask the public auth_path element into the right half
        randombytes(auth_mask, SPX_N);
        for (int j=0; j<SPX_N; j++) {
            buffer1[SPX_N + j] = auth_path[j] ^ auth_mask[j];
            buffer2[SPX_N + j] = auth_mask[j];
        }
    }
    auth_path += SPX_N;

    for (i = 0; i < tree_height - 1; i++) {
        leaf_idx >>= 1;
        idx_offset >>= 1;
        
        set_tree_height(addr, i + 1);
        set_tree_index(addr, leaf_idx + idx_offset);

        // Pick the right or left neighbor, depending on parity of the node. 
        if (leaf_idx & 1) {
            // Output of thash goes to the right half of the buffer.
            masked_thash(buffer1 + SPX_N, buffer2 + SPX_N, // output
                         buffer1, buffer2,                 // input (Left||Right)
                         2, ctx, addr);
            
            // Randomly mask the next auth_path element into the left half
            randombytes(auth_mask, SPX_N);
            for (int j=0; j<SPX_N; j++) {
                buffer1[j] = auth_path[j] ^ auth_mask[j];
                buffer2[j] = auth_mask[j];
            }
        }
        else {
            // The previous hash result is the left child.
            masked_thash(buffer1, buffer2, 
                         buffer1, buffer2, 
                         2, ctx, addr);

            // Randomly mask the next auth_path element into the right half
            randombytes(auth_mask, SPX_N);
            for (int j=0; j<SPX_N; j++) {
                buffer1[SPX_N + j] = auth_path[j] ^ auth_mask[j];
                buffer2[SPX_N + j] = auth_mask[j];
            }
        }
        auth_path += SPX_N;
    }

    // The last iteration doesnt require any auth path copying 
    leaf_idx >>= 1;
    idx_offset >>= 1;
    set_tree_height(addr, tree_height);
    set_tree_index(addr, leaf_idx + idx_offset);
    
    // Final hash to get the root shares
    masked_thash(root1, root2, buffer1, buffer2, 2, ctx, addr);
}

//Masked version of treehash
void treehash_masked(unsigned char *root1, unsigned char *root2, 
                     unsigned char *auth_path, 
                     const spx_ctx* ctx,
                     uint32_t leaf_idx, uint32_t idx_offset, 
                     uint32_t tree_height,
                     void (*gen_leaf_masked)(
                        unsigned char* , // leaf share 1
                        unsigned char* , // leaf share 2
                        const spx_ctx* ,
                        uint32_t , void *info),
                        uint32_t tree_addr[8],
                     void *info)
{
    // Two stacks for each share
    SPX_VLA(uint8_t, stack1, (tree_height+1)*SPX_N);
    SPX_VLA(uint8_t, stack2, (tree_height+1)*SPX_N);
    
    SPX_VLA(unsigned int, heights, tree_height+1);
    unsigned int offset = 0;
    uint32_t idx;
    uint32_t tree_idx;

    // Temporary buffers for hashing
    unsigned char in_buf1[2 * SPX_N];
    unsigned char in_buf2[2 * SPX_N];

    for (idx = 0; idx < (uint32_t)(1 << tree_height); idx++) {
        
        // Generate the next leaf node  directly onto the stack 
        gen_leaf_masked(stack1 + offset*SPX_N, 
                        stack2 + offset*SPX_N, 
                        ctx, idx + idx_offset, info);
                        
        offset++;
        heights[offset - 1] = 0;
        if (auth_path != NULL && (leaf_idx ^ 0x1) == idx) { // If this is a node we need for the auth path
            for(int j=0; j<SPX_N; j++) {
                auth_path[j] = stack1[(offset - 1)*SPX_N + j] ^ stack2[(offset - 1)*SPX_N + j];
            }
        }

        while (offset >= 2 && heights[offset - 1] == heights[offset - 2]) { // While top two nodes are of equal height
            tree_idx = (idx >> (heights[offset - 1] + 1));

            // Set the address of the node it is being createed
            set_tree_height(tree_addr, heights[offset - 1] + 1);
            set_tree_index(tree_addr,
                           tree_idx + (idx_offset >> (heights[offset-1] + 1)));
            

            // prepare input buffers for masked thash
            // In1 = Left1 || Right1
            memcpy(in_buf1, stack1 + (offset - 2)*SPX_N, SPX_N);
            memcpy(in_buf1 + SPX_N, stack1 + (offset - 1)*SPX_N, SPX_N);

            // In2 = Left2 || Right2
            memcpy(in_buf2, stack2 + (offset - 2)*SPX_N, SPX_N);
            memcpy(in_buf2 + SPX_N, stack2 + (offset - 1)*SPX_N, SPX_N);

            // Hash the top-most masked nodes 
            masked_thash(stack1 + (offset - 2)*SPX_N, // Out1
                         stack2 + (offset - 2)*SPX_N, // Out2
                         in_buf1, in_buf2,            // In1, In2
                         2, ctx, tree_addr);

            offset--;
            // Top-most node is now one layer higher
            heights[offset - 1]++;

            if (auth_path != NULL && ((leaf_idx >> heights[offset - 1]) ^ 0x1) == tree_idx) { // Might be needed for auth path again
                for(int j=0; j<SPX_N; j++) {
                    auth_path[heights[offset - 1]*SPX_N + j] = 
                        stack1[(offset - 1)*SPX_N + j] ^ stack2[(offset - 1)*SPX_N + j];
                }
            }
        }
    }
    
    // Copy result to output pointers
    memcpy(root1, stack1, SPX_N);
    memcpy(root2, stack2, SPX_N);
}
