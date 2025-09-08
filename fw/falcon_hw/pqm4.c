#include <stddef.h>
#include <string.h>

#include "api.h"
#include "inner.h"
#include "randombytes.h"
#include <stdio.h>
/* ==================================================================== */

/*
 * Falcon degree is N = 2^LOGN, where LOGN=9 (for Falcon-512) or 10
 * (for Falcon-1024). We use the advertised public key size to know
 * which degree is used.
 */
#if CRYPTO_PUBLICKEYBYTES == 897
#define LOGN   9
#elif CRYPTO_PUBLICKEYBYTES == 1793
#define LOGN   10
#else
#error Unknown Falcon degree (unexpected public key size)
#endif

#define N   ((size_t)1 << LOGN)
#define NONCELEN   40
#define SEEDLEN    48

/*
 * If the private key length is larger than 10000, then this is the
 * variant with precomputed expanded keys.
 */
#if CRYPTO_SECRETKEYBYTES > 10000
#define KG_EXPAND   1
#else
#define KG_EXPAND   0
#endif

/*
 * Common buffer, to avoid bulky stack allocation. The buffer sizes are
 * all expressed in bytes, but the buffer must be suitably aligned for
 * 64-bit integers and floating-point values.
 *
 * Required size (in bytes):
 *
 *   With expanded key:
 *      keygen:  48*N + 6*N = 54*N
 *      sign:    48*N + 2*N = 50*N
 *      vrfy:    8*N
 *
 *   Without expanded key:
 *      keygen:  28*N + 5*N = 33*N
 *      sign:    72*N + 6*N = 78*N
 *      vrfy:    8*N
 */
static union {
#if KG_EXPAND
	uint8_t b[54 * N];
#else
	uint8_t b[78 * N];
#endif
	uint64_t dummy_u64;
	fpr dummy_fp;
} tmp;


int
crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
	int8_t *f, *g, *F, *G;
	uint16_t *h;
	inner_shake256_context rng;
	unsigned char seed[SEEDLEN];
#if KG_EXPAND
	size_t v;
#else
	size_t u, v;
#endif
	unsigned sav_cw;

#if KG_EXPAND
	f = (int8_t *)&tmp.b[48 * N];
	g = f + N;
	F = g + N;
	G = F + N;
	h = (uint16_t *)(G + N);
#else
	f = (int8_t *)&tmp.b[28 * N];
	g = f + N;
	F = g + N;
	G = NULL;
	h = (uint16_t *)(F + N);
#endif

	randombytes(seed, SEEDLEN);
	inner_shake256_init(&rng);
	inner_shake256_inject(&rng, seed, SEEDLEN);
	inner_shake256_flip(&rng);
	sav_cw = set_fpu_cw(2);
	Zf(keygen)(&rng, f, g, F, G, h, LOGN, tmp.b);

#if KG_EXPAND
	/*
	 * Expand private key.
	 */
	Zf(expand_privkey)((fpr *)sk, f, g, F, G, LOGN, tmp.b);
	set_fpu_cw(sav_cw);
#else
	set_fpu_cw(sav_cw);

	/*
	 * Encode private key.
	 */
	sk[0] = 0x50 + LOGN;
	u = 1;
	v = Zf(trim_i8_encode)(sk + u, CRYPTO_SECRETKEYBYTES - u,
		f, LOGN, Zf(max_fg_bits)[LOGN]);
	if (v == 0) {
		return -1;
	}
	u += v;
	v = Zf(trim_i8_encode)(sk + u, CRYPTO_SECRETKEYBYTES - u,
		g, LOGN, Zf(max_fg_bits)[LOGN]);
	if (v == 0) {
		return -1;
	}
	u += v;
	v = Zf(trim_i8_encode)(sk + u, CRYPTO_SECRETKEYBYTES - u,
		F, LOGN, Zf(max_FG_bits)[LOGN]);
	if (v == 0) {
		return -1;
	}
	u += v;
	if (u != CRYPTO_SECRETKEYBYTES) {
		return -1;
	}
#endif

	/*
	 * Encode public key.
	 */
	pk[0] = 0x00 + LOGN;
	v = Zf(modq_encode)(pk + 1, CRYPTO_PUBLICKEYBYTES - 1, h, LOGN);
	if (v != CRYPTO_PUBLICKEYBYTES - 1) {
		return -1;
	}

	return 0;
}

int
crypto_sign(unsigned char *sm, size_t *smlen,
	const unsigned char *m, size_t mlen,
	const unsigned char *sk)
{
#if KG_EXPAND
	const fpr *expanded_key;
#else
	int8_t *f, *g, *F, *G;
	size_t u, v;
#endif
	int16_t *sig;
	uint16_t *hm;
	unsigned char seed[SEEDLEN], nonce[NONCELEN];
	unsigned char *esig;
	inner_shake256_context sc;
	size_t sig_len;
	unsigned sav_cw;

#if KG_EXPAND
	sig = (int16_t *)&tmp.b[48 * N];
#else
	f = (int8_t *)&tmp.b[72 * N];
	g = f + N;
	F = g + N;
	G = F + N;
	sig = (int16_t *)(G + N);
#endif
	hm = (uint16_t *)sig;  /* hm[] is shared with sig[] */
	esig = (unsigned char *)tmp.b;

#if KG_EXPAND
	/*
	 * Expanded key is provided "as is".
	 */
	expanded_key = (const fpr *)sk;
#else
	/*
	 * Decode the private key.
	 */
	if (sk[0] != 0x50 + LOGN) {
		return -1;
	}
	u = 1;
	v = Zf(trim_i8_decode)(f, LOGN, Zf(max_fg_bits)[LOGN],
		sk + u, CRYPTO_SECRETKEYBYTES - u);
	if (v == 0) {
		return -1;
	}
	u += v;
	v = Zf(trim_i8_decode)(g, LOGN, Zf(max_fg_bits)[LOGN],
		sk + u, CRYPTO_SECRETKEYBYTES - u);
	if (v == 0) {
		return -1;
	}
	u += v;
	v = Zf(trim_i8_decode)(F, LOGN, Zf(max_FG_bits)[LOGN],
		sk + u, CRYPTO_SECRETKEYBYTES - u);
	if (v == 0) {
		return -1;
	}
	u += v;
	if (u != CRYPTO_SECRETKEYBYTES) {
		return -1;
	}
	if (!Zf(complete_private)(G, f, g, F, LOGN, tmp.b)) {
		return -1;
	}

	/*
	* Complete private key - dump test vectors
	*/
	printf("/* Generated complete_private test vectors from pqm4.c */\n");
	printf("/* Zf(complete_private)(G, f, g, F, LOGN, tmp.b) */\n\n");
	printf("#ifndef FALCON_COMPLETE_PRIVATE_VECTORS_H\n");
	printf("#define FALCON_COMPLETE_PRIVATE_VECTORS_H\n\n");
	printf("#include <stdint.h>\n\n");

	// Input: f polynomial
	printf("const int8_t complete_private_f[%zu] = {\n", (size_t)N);
	for (size_t i = 0; i < N; i++) {
		printf(" %d", (int8_t)f[i]);
		if (i < N - 1) printf(",");
	}
	printf("};\n\n");

	// Input: g polynomial  
	printf("const int8_t complete_private_g[%zu] = {\n", (size_t)N);
	for (size_t i = 0; i < N; i++) {
		printf(" %d", (int8_t)g[i]);
		if (i < N - 1) printf(",");
	}
	printf("};\n\n");

	// Input: F polynomial
	printf("const int8_t complete_private_F[%zu] = {\n", (size_t)N);
	for (size_t i = 0; i < N; i++) {
		printf(" %d", (int8_t)F[i]);
		if (i < N - 1) printf(",");
	}
	printf("};\n\n");

	// Output: G polynomial (computed by complete_private)
	printf("const int8_t complete_private_G[%zu] = {\n", (size_t)N);
	for (size_t i = 0; i < N; i++) {
		printf(" %d", (int8_t)G[i]);
		if (i < N - 1) printf(",");
	}
	printf("};\n\n");

	printf("#endif /* FALCON_COMPLETE_PRIVATE_VECTORS_H */\n");
#endif

	/*
	 * Create a random nonce (40 bytes).
	 */
	randombytes(nonce, NONCELEN);

	/*
	 * Hash message nonce + message into a vector.
	 */
	inner_shake256_init(&sc);
	inner_shake256_inject(&sc, nonce, NONCELEN);
	inner_shake256_inject(&sc, m, mlen);
	inner_shake256_flip(&sc);
	Zf(hash_to_point_vartime)(&sc, hm, LOGN);

	/*
	 * Initialize a RNG.
	 */
	randombytes(seed, SEEDLEN);
	inner_shake256_init(&sc);
	inner_shake256_inject(&sc, seed, SEEDLEN);
	inner_shake256_flip(&sc);

	/*
	 * Compute the signature.
	 */
	sav_cw = set_fpu_cw(2);
#if KG_EXPAND
	Zf(sign_tree)(sig, &sc, expanded_key, hm, LOGN, tmp.b);
#else
	Zf(sign_dyn)(sig, &sc, f, g, F, G, hm, LOGN, tmp.b);
#endif
	set_fpu_cw(sav_cw);

	/*
	 * Encode the signature and bundle it with the message. Format is:
	 *   signature length     2 bytes, big-endian
	 *   nonce                40 bytes
	 *   message              mlen bytes
	 *   signature            slen bytes
	 */
	esig[0] = 0x20 + LOGN;
	sig_len = Zf(comp_encode)(esig + 1, CRYPTO_BYTES - 1, sig, LOGN);
	if (sig_len == 0) {
		return -1;
	}
	sig_len ++;
	memmove(sm + 2 + NONCELEN, m, mlen);
	sm[0] = (unsigned char)(sig_len >> 8);
	sm[1] = (unsigned char)sig_len;
	memcpy(sm + 2, nonce, NONCELEN);
	memcpy(sm + 2 + NONCELEN + mlen, esig, sig_len);
	*smlen = 2 + NONCELEN + mlen + sig_len;
	return 0;
}

int
crypto_sign_open(unsigned char *m, size_t *mlen,
	const unsigned char *sm, size_t smlen,
	const unsigned char *pk)
{
	uint16_t *h, *hm;
	int16_t *sig;
	const unsigned char *esig;
	inner_shake256_context sc;
	size_t sig_len, msg_len;

	h = (uint16_t *)&tmp.b[2 * N];
	hm = h + N;
	sig = (int16_t *)(hm + N);

	/*
	 * Decode public key.
	 */
	if (pk[0] != 0x00 + LOGN) {
		return -1;
	}
	if (Zf(modq_decode)(h, LOGN, pk + 1, CRYPTO_PUBLICKEYBYTES - 1)
		!= CRYPTO_PUBLICKEYBYTES - 1)
	{
		return -1;
	}
	
	// NTT öncesi h polinomunu kaydet
	uint16_t h_before_ntt[N];
	memcpy(h_before_ntt, h, N * sizeof(uint16_t));

	Zf(to_ntt_monty)(h, LOGN);


	/*
	* NTT Monty test vectors
	*/
	printf("/* Generated NTT Monty test vectors from pqm4.c */\n");
	printf("/* Zf(to_ntt_monty)(h, LOGN) */\n\n");
	printf("#ifndef FALCON_NTT_MONTY_VECTORS_H\n");
	printf("#define FALCON_NTT_MONTY_VECTORS_H\n\n");
	printf("#include <stdint.h>\n\n");

	// Input: h polynomial before NTT (decoded from public key)
	printf("// Input: h polynomial (decoded from public key, before NTT)\n");
	printf("const uint16_t ntt_input_h[%zu] = {\n", (size_t)N);
	for (size_t i = 0; i < N; i++) {
		printf(" %u", (unsigned)h_before_ntt[i]);
		if (i < N - 1) printf(",");
		if ((i + 1) % 8 == 0) printf("\n");
	}
	printf("};\n\n");

	// Output: h polynomial after NTT Monty transformation
	printf("// Output: h polynomial (after NTT Monty transformation)\n");
	printf("const uint16_t ntt_output_h[%zu] = {\n", (size_t)N);
	for (size_t i = 0; i < N; i++) {
		printf(" %u", (unsigned)h[i]);
		if (i < N - 1) printf(",");
		if ((i + 1) % 8 == 0) printf("\n");
	}
	printf("};\n\n");

	printf("#endif /* FALCON_NTT_MONTY_VECTORS_H */\n");

	/*
	 * Find nonce, signature, message length.
	 */
	if (smlen < 2 + NONCELEN) {
		return -1;
	}
	sig_len = ((size_t)sm[0] << 8) | (size_t)sm[1];
	if (sig_len > (smlen - 2 - NONCELEN)) {
		return -1;
	}
	msg_len = smlen - 2 - NONCELEN - sig_len;

	/*
	 * Decode signature.
	 */
	esig = sm + 2 + NONCELEN + msg_len;
	if (sig_len < 1 || esig[0] != 0x20 + LOGN) {
		return -1;
	}
	if (Zf(comp_decode)(sig, LOGN,
		esig + 1, sig_len - 1) != sig_len - 1)
	{
		return -1;
	}

	/*
	 * Hash nonce + message into a vector.
	 */
	inner_shake256_init(&sc);
	inner_shake256_inject(&sc, sm + 2, NONCELEN + msg_len);
	inner_shake256_flip(&sc);
	Zf(hash_to_point_vartime)(&sc, hm, LOGN);

    /*
     * Verify signature – dump test vectors
     */
    printf("/* Generated test vectors from pqm4.c */\n");
	printf("/*Zf(verify_raw)(hm, sig, h, LOGN, tmp.b)*/\n\n");
    printf("#ifndef FALCON_VECTORS_H\n");
    printf("#define FALCON_VECTORS_H\n\n");
    printf("#include <stdint.h>\n\n");
    // Public key h
    printf("const uint16_t verify_raw_h[%zu] = {\n", (size_t)N);
    for (size_t i = 0; i < N; i++) {
        printf(" %u", (unsigned)h[i]);
        if (i < N - 1) printf(",");
    }
    printf("};\n\n");

    // Signature s1 || s2
    printf("const int16_t verify_raw_sig[%zu] = {\n", (size_t)2*N);
    for (size_t i = 0; i < 2*N; i++) {
        printf(" %d", (int16_t)sig[i]);
        if (i < 2*N - 1) printf(",");
    }
    printf("};\n\n");

    // Challenge point c0 (hm)
    printf("const uint16_t verify_raw_hm[%zu] = {\n", (size_t)N);
    for (size_t i = 0; i < N; i++) {
        printf(" %u", (unsigned)hm[i]);
        if (i < N - 1) printf(",");
    }
    printf("};\n\n");

    printf("#endif /* FALCON_VECTORS_H */\n");

	/*
	 * Verify signature.
	 */
	if (!Zf(verify_raw)(hm, sig, h, LOGN, tmp.b)) {
		return -1;
	}
	printf("\nSUCCESS VERIFY\n");
	/*
	 * Return plaintext.
	 */
	memmove(m, sm + 2 + NONCELEN, msg_len);
	*mlen = msg_len;
	return 0;
}

/*
 * Yeni eklenen fonksiyon: verify_recover ve count_nttzero ile imza doğrulama
 * Sadece test ve debug için - normal doğrulama için crypto_sign_open kullanın
 */
int crypto_sign_recover(const unsigned char *sm, size_t smlen, const unsigned char *pk)
{
    uint16_t *h, *hm;
    int16_t *sig;
    const unsigned char *esig;
    inner_shake256_context sc;
    size_t sig_len, msg_len;

    // Geçici bellek ayırma (crypto_sign_open ile aynı)
    h = (uint16_t *)&tmp.b[2 * N];
    hm = h + N;
    sig = (int16_t *)(hm + N);

    /*
     * Decode public key.
     */
    if (pk[0] != 0x00 + LOGN) {
        printf("Invalid public key format\n");
        return -1;
    }
    if (Zf(modq_decode)(h, LOGN, pk + 1, CRYPTO_PUBLICKEYBYTES - 1)
        != CRYPTO_PUBLICKEYBYTES - 1)
    {
        printf("Public key decode failed\n");
        return -1;
    }
    Zf(to_ntt_monty)(h, LOGN);

    /*
     * Find nonce, signature, message length.
     */
    if (smlen < 2 + NONCELEN) {
        printf("Message too short\n");
        return -1;
    }
    sig_len = ((size_t)sm[0] << 8) | (size_t)sm[1];
    if (sig_len > (smlen - 2 - NONCELEN)) {
        printf("Invalid signature length\n");
        return -1;
    }
    msg_len = smlen - 2 - NONCELEN - sig_len;

    /*
     * Decode signature.
     */
    esig = sm + 2 + NONCELEN + msg_len;
    if (sig_len < 1 || esig[0] != 0x20 + LOGN) {
        printf("Invalid signature format\n");
        return -1;
    }
    if (Zf(comp_decode)(sig, LOGN, esig + 1, sig_len - 1) != sig_len - 1)
    {
        printf("Signature decode failed\n");
        return -1;
    }

    /*
     * Hash nonce + message into a vector.
     */
    inner_shake256_init(&sc);
    inner_shake256_inject(&sc, sm + 2, NONCELEN + msg_len);
    inner_shake256_flip(&sc);
    Zf(hash_to_point_vartime)(&sc, hm, LOGN);

    /*
     * verify_recover ve count_nttzero testleri
     */
    printf("\n=== crypto_sign_recover Test Results ===\n");

    // Test verify_recover function
    uint16_t h_recovered[N];
    int verify_result = Zf(verify_recover)(h_recovered, hm, sig, sig + N, LOGN, tmp.b);

    // Test count_nttzero function
    int zero_count = Zf(count_nttzero)(sig + N, LOGN, tmp.b);

    /*
     * Detaylı test vektörü çıktıları
     */
    printf("/* Generated verify_recover test vectors */\n");
    printf("/* Zf(verify_recover) and Zf(count_nttzero) */\n\n");
    printf("#ifndef FALCON_RECOVER_VECTORS_H\n");
    printf("#define FALCON_RECOVER_VECTORS_H\n\n");
    printf("#include <stdint.h>\n\n");

    // Input: Challenge point c0 (hm)
    printf("const uint16_t recover_c0[%zu] = {\n", (size_t)N);
    for (size_t i = 0; i < N; i++) {
        printf(" %u", (unsigned)hm[i]);
        if (i < N - 1) printf(",");
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("};\n\n");

    // Input: Signature s1 (first half of sig)
    printf("const int16_t recover_s1[%zu] = {\n", (size_t)N);
    for (size_t i = 0; i < N; i++) {
        printf(" %d", (int16_t)sig[i]);
        if (i < N - 1) printf(",");
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("};\n\n");

    // Input: Signature s2 (second half of sig)
    printf("const int16_t recover_s2[%zu] = {\n", (size_t)N);
    for (size_t i = 0; i < N; i++) {
        printf(" %d", (int16_t)sig[N + i]);
        if (i < N - 1) printf(",");
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("};\n\n");

    // Input: Public key h (after NTT)
    printf("const uint16_t recover_h_ntt[%zu] = {\n", (size_t)N);
    for (size_t i = 0; i < N; i++) {
        printf(" %u", (unsigned)h[i]);
        if (i < N - 1) printf(",");
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("};\n\n");

    // Output: Recovered public key from verify_recover
    printf("const uint16_t recover_h_recovered[%zu] = {\n", (size_t)N);
    for (size_t i = 0; i < N; i++) {
        printf(" %u", (unsigned)h_recovered[i]);
        if (i < N - 1) printf(",");
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("};\n\n");

    // Output: Results
    printf("const int recover_verify_result = %d;\n", verify_result);
    printf("const int recover_ntt_zero_count = %d;\n", zero_count);

    printf("#endif /* FALCON_RECOVER_VECTORS_H */\n\n");

    /*
     * Sonuçları ekrana yazdır
     */
    printf("=== Recovery Test Results ===\n");
    printf("verify_recover result: %d (%s)\n", 
           verify_result, verify_result == 1 ? "SUCCESS" : "FAILED");
    printf("count_nttzero result: %d zero coefficients\n", zero_count);

    // Kurtarılan public key'in orijinal ile karşılaştırması
    int match = 1;
    for (size_t i = 0; i < N; i++) {
        if (h_recovered[i] != h[i]) {
            match = 0;
            break;
        }
    }
    printf("Recovered h matches original h: %s\n", match ? "YES" : "NO");

    // verify_raw ile karşılaştırma
    int raw_result = Zf(verify_raw)(hm, sig, h, LOGN, tmp.b);
    printf("verify_raw result: %d (%s)\n", 
           raw_result, raw_result == 1 ? "SUCCESS" : "FAILED");

    printf("========================================\n\n");

    return verify_result;  // verify_recover sonucunu döndür
}