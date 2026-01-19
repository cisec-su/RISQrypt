#include <stdint.h>
#include <string.h>

#include "utils_masked.h"
#include "utilsx1_masked.h"
#include "hash_masked.h"
#include "thash_masked.h"
#include "wots_masked.h"
#include "wotsx1_masked.h"
#include "address.h"
#include "params.h"
#include "randombytes.h"

// Non-masked functions from original file
static void base_w(unsigned int *output, const int out_len,
                   const unsigned char *input)
{
    int in = 0;
    int out = 0;
    unsigned char total;
    int bits = 0;
    int consumed;

    for (consumed = 0; consumed < out_len; consumed++) {
        if (bits == 0) {
            total = input[in];
            in++;
            bits += 8;
        }
        bits -= SPX_WOTS_LOGW;
        output[out] = (total >> bits) & (SPX_WOTS_W - 1);
        out++;
    }
}

/* Computes the WOTS+ checksum over a message (in base_w). */
static void wots_checksum(unsigned int *csum_base_w,
                          const unsigned int *msg_base_w)
{
    unsigned int csum = 0;
    unsigned char csum_bytes[(SPX_WOTS_LEN2 * SPX_WOTS_LOGW + 7) / 8];
    unsigned int i;

    /* Compute checksum. */
    for (i = 0; i < SPX_WOTS_LEN1; i++) {
        csum += SPX_WOTS_W - 1 - msg_base_w[i];
    }

    /* Convert checksum to base_w. */
    /* Make sure expected empty zero bits are the least significant bits. */
    csum = csum << ((8 - ((SPX_WOTS_LEN2 * SPX_WOTS_LOGW) % 8)) % 8);
    ull_to_bytes(csum_bytes, sizeof(csum_bytes), csum);
    base_w(csum_base_w, SPX_WOTS_LEN2, csum_bytes);
}

/* Takes a message and derives the matching chain lengths. */
void chain_lengths(unsigned int *lengths, const unsigned char *msg)
{
    base_w(lengths, SPX_WOTS_LEN1, msg);
    wots_checksum(lengths + SPX_WOTS_LEN1, lengths);
}

// Masked version of gen_chain
static void gen_chain_masked(unsigned char *out1, const unsigned char *out2,
                      const unsigned char *in1, const unsigned char *in2,
                      unsigned int start, unsigned int steps,
                      const spx_ctx *ctx, uint32_t addr[8])
{
    uint32_t i;

    // Initialize out with the value at position start
    memcpy(out1, in1, SPX_N);
    memcpy(out2, in2, SPX_N);

    // Iterate steps calls to the hash function
    for (i = start; i < (start+steps) && i < SPX_WOTS_W; i++) {
        set_hash_addr(addr, i); 
        masked_thash(out1, out2, out1, out2, 1, ctx, addr); // Output shares, Input shares, 1 block WOTS hashing
    }
}

// Masked version of wots_pk_from_sig
void wots_pk_from_sig_masked(unsigned char *pk,
                             const unsigned char *sig, 
                             const unsigned char *msg,
                             const spx_ctx *ctx, uint32_t addr[8])
{
    unsigned int lengths[SPX_WOTS_LEN];
    uint32_t i;
    
    // Output shares for the calculated Public Key leaf
    unsigned char pk_share1[SPX_N];
    unsigned char pk_share2[SPX_N];

    // Input shares for the Signature
    unsigned char sig_share1[SPX_N];
    unsigned char sig_share2[SPX_N];

    // Calculate chain lengths 
    chain_lengths(lengths, msg);

    for (i = 0; i < SPX_WOTS_LEN; i++) {
        set_chain_addr(addr, i);

        randombytes(sig_share2, SPX_N); // Random share for signature
        
        // Split signature into 2 shares
        for(int j=0; j<SPX_N; j++) {
            sig_share1[j] = sig[i*SPX_N + j] ^ sig_share2[j]; // sig ^r, r
        }
        // Generate masked chain 
        gen_chain_masked(pk_share1, pk_share2,    // Outputs
                         sig_share1, sig_share2,  // Inputs
                         lengths[i],              // Start index
                         SPX_WOTS_W - 1 - lengths[i], // Steps
                         ctx, addr);
        
        // Recombine output to get the Public Key
        for(int j=0; j<SPX_N; j++) {
            pk[i*SPX_N + j] = pk_share1[j] ^ pk_share2[j]; // pk = share1 ^ share2
        }
    }
}
