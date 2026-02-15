#include <stdint.h>
#include <string.h>

#include "swmasked_utils.h"
#include "utils.h"
#include "swmasked_utilsx1.h"
#include "swmasked_hash.h"
#include "swmasked_thash.h"
#include "swmasked_wots.h"
#include "swmasked_wotsx1.h"
#include "address.h"
#include "params.h"
#include "randombytes.h"

// Non-masked functions removed (provided by wots.c)

// Masked version of gen_chain
static void gen_chain_masked(unsigned char *out1, unsigned char *out2,
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
        swmasked_thash(out1, out2, out1, out2, 1, ctx, addr); // Output shares, Input shares, 1 block WOTS hashing
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
