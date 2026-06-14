#ifndef masked_SPX_WOTS_H
#define masked_SPX_WOTS_H

#include <stdint.h>

#include "params.h"
#include "masked_context.h"

#define masked_wots_pk_from_sig SPX_NAMESPACE(masked_wots_pk_from_sig)
void masked_wots_pk_from_sig(unsigned char *pk,
                               const unsigned char *sig,
                               const unsigned char *msg,
                               const spx_ctx *ctx, uint32_t addr[8]);

/*
 * Compute the chain lengths needed for a given message hash
 */
#define chain_lengths SPX_NAMESPACE(chain_lengths)
void chain_lengths(unsigned int *lengths, const unsigned char *msg);

#endif
