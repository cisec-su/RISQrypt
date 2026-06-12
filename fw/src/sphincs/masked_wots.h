#ifndef SPX_WOTS_HWMASKED_H
#define SPX_WOTS_HWMASKED_H

#include <stdint.h>

#include "params.h"
#include "masked_context.h"

#define wots_pk_from_sig_hwmasked SPX_NAMESPACE(wots_pk_from_sig_hwmasked)
void wots_pk_from_sig_hwmasked(unsigned char *pk,
                               const unsigned char *sig,
                               const unsigned char *msg,
                               const spx_ctx *ctx, uint32_t addr[8]);

/*
 * Compute the chain lengths needed for a given message hash
 */
#define chain_lengths SPX_NAMESPACE(chain_lengths)
void chain_lengths(unsigned int *lengths, const unsigned char *msg);

#endif
