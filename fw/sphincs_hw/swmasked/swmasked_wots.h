#ifndef SPX_WOTS_swmasked_H
#define SPX_WOTS_swmasked_H

#include <stdint.h>

#include "params.h"
#include "swmasked_context.h"

/**
 * Takes a WOTS signature and an n-byte message, computes a WOTS public key.
 *
 * Writes the computed public key to 'pk'.
 */
#define wots_pk_from_sig_masked SPX_NAMESPACE(wots_pk_from_sig_masked)
void wots_pk_from_sig_masked(unsigned char *pk,
                             const unsigned char *sig, 
                             const unsigned char *msg,
                             const spx_ctx *ctx, uint32_t addr[8]);

/*
 * Compute the chain lengths needed for a given message hash
 */
#define chain_lengths SPX_NAMESPACE(chain_lengths)
void chain_lengths(unsigned int *lengths, const unsigned char *msg);

#endif
