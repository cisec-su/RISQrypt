/*
 * Implementation of the external Falcon API.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2017-2019  Falcon Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@nccgroup.com>
 */

#include "falcon.h"
#include "symmetric.h" 
#include "poly.h"
#include "verify.h"



/* ================================================================== */
/* PUBLIC API WRAPPERS                                                */
/* ================================================================== */

void shake256_init() { 
    falcon_shake256_init();
}

void shake256_inject(const void *data, size_t len) { 
    falcon_shake256_absorb((const uint8_t *)data, len); 
}

void shake256_flip() { 
    falcon_shake256_finish(); 
}

void shake256_extract(void *out, size_t len) { 
    falcon_shake256_squeeze((uint8_t *)out, len); 
}

void shake256_init_prng_from_seed(const void *seed, size_t seed_len) { 
    shake256_init();
    shake256_inject(seed, seed_len); 
    shake256_flip(); 
}


/* see falcon.h */
int falcon_verify_start(const void *sig, size_t sig_len) {
    if (sig_len < 41) {
        return FALCON_ERR_FORMAT;
    }

    shake256_init();
    shake256_inject((const uint8_t *)sig + 1, 40);

    return 0;
}

/* see falcon.h */
int falcon_verify_finish(const void *sig, size_t sig_len, int sig_type,
    const void *pubkey, size_t pubkey_len) {

    unsigned logn;
    uint8_t *atmp;
    const uint8_t *pk, *es;
    size_t u, v, n;
    int ct;
    poly h, hm, sv;

    /*
     * Get Falcon degree from public key; verify consistency with
     * signature value, and check parameters.
     */
    if (sig_len < 41 || pubkey_len == 0) {
        return FALCON_ERR_FORMAT;
    }
    es = sig;
    pk = pubkey;
    if ((pk[0] & 0xF0) != 0x00) {
        return FALCON_ERR_FORMAT;
    }
    logn = pk[0] & 0x0F;
    if (logn < 1 || logn > 10) {
        return FALCON_ERR_FORMAT;
    }
    if ((es[0] & 0x0F) != logn) {
        return FALCON_ERR_BADSIG;
    }
    
    ct = 0;
    switch (sig_type) {
    case 0: /* Auto-detect */
        switch (es[0] & 0xF0) {
        case 0x30:
            break;
        case 0x50:
            if (sig_len != FALCON_SIG_CT_SIZE(logn)) {
                return FALCON_ERR_FORMAT;
            }
            ct = 1;
            break;
        default:
            return FALCON_ERR_BADSIG;
        }
        break;
    case FALCON_SIG_COMPRESSED:
        if ((es[0] & 0xF0) != 0x30) {
            return FALCON_ERR_FORMAT;
        }
        break;
    case FALCON_SIG_PADDED:
        if ((es[0] & 0xF0) != 0x30) {
            return FALCON_ERR_FORMAT;
        }
        if (sig_len != FALCON_SIG_PADDED_SIZE(logn)) {
            return FALCON_ERR_FORMAT;
        }
        break;
    case FALCON_SIG_CT:
        if ((es[0] & 0xF0) != 0x50) {
            return FALCON_ERR_FORMAT;
        }
        if (sig_len != FALCON_SIG_CT_SIZE(logn)) {
            return FALCON_ERR_FORMAT;
        }
        ct = 1;
        break;
    default:
        return FALCON_ERR_BADARG;
    }

    if (ct) {
        return FALCON_ERR_BADARG; // unsupported parameter set
    }


    if (pubkey_len != FALCON_PUBKEY_SIZE(logn)) {
        return FALCON_ERR_FORMAT;
    }

    n = (size_t)1 << logn;

    if (n != N) {
        return FALCON_ERR_BADARG; // unsupported parameter set
    }

    poly_init_q();
    /*
     * Decode public key.
     */
    if (poly_modq_decode(&h, pk + 1, pubkey_len - 1))
    {
        return FALCON_ERR_FORMAT;
    }

    /*
     * Decode signature value.
     */
    u = 41;
    v = poly_decompress(&sv, es + u, sig_len - u);
    
    if (v == 0) {
        return FALCON_ERR_FORMAT;
    }
    
    if ((u + v) != sig_len) {
        /*
         * Extra bytes of value 0 are tolerated only for the
         * "padded" format.
         */
        if ((sig_type == 0 && sig_len == FALCON_SIG_PADDED_SIZE(logn))
            || sig_type == FALCON_SIG_PADDED)
        {
            while (u + v < sig_len) {
                if (es[u + v] != 0) {
                    return FALCON_ERR_FORMAT;
                }
                v ++;
            }
        } else {
            return FALCON_ERR_FORMAT;
        }
    }

    /*
     * Hash message to point.
     */

    shake256_flip();

    poly_hash_to_point(&hm);

    /*
     * Verify signature.
     */
    if (verify_raw(&hm, &sv, &h)) {
        return FALCON_ERR_BADSIG;
    }
    
    return 0;
}

/* see falcon.h */
int falcon_verify(const void *sig, size_t sig_len, int sig_type,
    const void *pubkey, size_t pubkey_len,
    const void *data, size_t data_len)
{
    int r;
    r = falcon_verify_start(sig, sig_len);
    if (r < 0) {
        return r;
    }

    shake256_inject(data, data_len);
    
    return falcon_verify_finish(sig, sig_len, sig_type,
        pubkey, pubkey_len);
}
