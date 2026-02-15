#ifndef SPX_HWMASKED_FORS_H
#define SPX_HWMASKED_FORS_H

#include <stdint.h>

#include "params.h"
#include "swmasked_context.h"

#define fors_sign_hwmasked SPX_NAMESPACE(fors_sign_hwmasked)
void fors_sign_hwmasked(unsigned char *sig, unsigned char *pk,
                        const unsigned char *m,
                        const spx_ctx* ctx,
                        const uint32_t fors_addr[8]);

#define fors_pk_from_sig_hwmasked SPX_NAMESPACE(fors_pk_from_sig_hwmasked)
void fors_pk_from_sig_hwmasked(unsigned char *pk,
                               const unsigned char *sig, const unsigned char *m,
                               const spx_ctx* ctx,
                               const uint32_t fors_addr[8]);

#endif
