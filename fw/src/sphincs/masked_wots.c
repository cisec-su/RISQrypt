#include <stdint.h>
#include <string.h>

#include "masked_utils.h"
#include "utils.h"
#include "masked_utilsx1.h"
#include "masked_hash.h"
#include "masked_thash.h"
#include "masked_wots.h"
#include "masked_wotsx1.h"
#include "address.h"
#include "params.h"
#include "randombytes.h"

// HW masked version of gen_chain
static void masked_gen_chain(unsigned char *out1, unsigned char *out2,
                               const unsigned char *in1, const unsigned char *in2,
                               unsigned int start, unsigned int steps,
                               const spx_ctx *ctx, uint32_t addr[8])
{
    uint32_t i;

    memcpy(out1, in1, SPX_N);
    memcpy(out2, in2, SPX_N);

    for (i = start; i < (start+steps) && i < SPX_WOTS_W; i++) {
        set_hash_addr(addr, i);
        masked_thash(out1, out2, out1, out2, 1, ctx, addr);
    }
}

// HW masked version of wots_pk_from_sig
void masked_wots_pk_from_sig(unsigned char *pk,
                               const unsigned char *sig,
                               const unsigned char *msg,
                               const spx_ctx *ctx, uint32_t addr[8])
{
    unsigned int lengths[SPX_WOTS_LEN];
    uint32_t i;

    unsigned char pk_share1[SPX_N];
    unsigned char pk_share2[SPX_N];
    unsigned char sig_share1[SPX_N];
    unsigned char sig_share2[SPX_N];

    chain_lengths(lengths, msg);

    for (i = 0; i < SPX_WOTS_LEN; i++) {
        set_chain_addr(addr, i);

        randombytes(sig_share2, SPX_N);

        for(int j=0; j<SPX_N; j++) {
            sig_share1[j] = sig[i*SPX_N + j] ^ sig_share2[j];
        }

        masked_gen_chain(pk_share1, pk_share2,
                           sig_share1, sig_share2,
                           lengths[i],
                           SPX_WOTS_W - 1 - lengths[i],
                           ctx, addr);

        for(int j=0; j<SPX_N; j++) {
            pk[i*SPX_N + j] = pk_share1[j] ^ pk_share2[j];
        }
    }
}
