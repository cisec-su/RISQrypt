#ifndef masked_SPX_FORS_H
#define masked_SPX_FORS_H

#include <stdint.h>

#include "params.h"
#include "masked_context.h"

#define masked_fors_sign SPX_NAMESPACE(masked_fors_sign)
void masked_fors_sign(unsigned char *sig, unsigned char *pk,
                        const unsigned char *m,
                        const spx_ctx* ctx,
                        const uint32_t fors_addr[8]);

#define masked_fors_pk_from_sig SPX_NAMESPACE(masked_fors_pk_from_sig)
void masked_fors_pk_from_sig(unsigned char *pk,
                               const unsigned char *sig, const unsigned char *m,
                               const spx_ctx* ctx,
                               const uint32_t fors_addr[8]);

#endif
