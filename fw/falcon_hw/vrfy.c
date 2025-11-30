/*
 * Falcon signature verification.
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

#include "inner.h"
#include "ntt_lite.h"
#include "uart.h"
#include "util.h"
/* ===================================================================== */
/*
 * Constants for NTT.
 *
 *   n = 2^logn  (2 <= n <= 1024)
 *   phi = X^n + 1
 *   q = 12289
 *   q0i = -1/q mod 2^16
 *   R = 2^16 mod q
 *   R2 = 2^32 mod q
 */

#define Q     12289
#define Q0I   12287
#define R      4091
#define R2    10952


/* ===================================================================== */
/* Corrected Tables based on test_gen.py logic */
/* Generator: psi=49 (1024-th root), Q=12289, N=256 tables */

// Falcon-512 Forward NTT Twiddles (GMb) for DUAL mode
// logn=8, n=256, q=12289

/*Falcon-512 Configuration:
q = 12289 (0x3001)
psi = 7
logn = 8, n = 256
inv2 = 6145 (0x1801)
mu_32 = 0x55538*/

const uint32_t GMb[] = {
        0x2ae8, 0x0dd6, 0x2993, 0x1d2c, 0x2dce, 0x1660, 0x2301, 0x090f, 
        0x2380, 0x12ee, 0x12c5, 0x0c13, 0x246e, 0x2b68, 0x2f75, 0x04ec, 
        0x097a, 0x07dd, 0x0b41, 0x2193, 0x0d36, 0x0e35, 0x2556, 0x24e7, 
        0x265d, 0x2978, 0x1fb0, 0x2c48, 0x09ac, 0x0f6d, 0x1eff, 0x0961, 
        0x0186, 0x0162, 0x1394, 0x03f9, 0x001b, 0x0601, 0x258b, 0x03ea, 
        0x1c91, 0x267c, 0x25fb, 0x2bd8, 0x0490, 0x01ed, 0x1f10, 0x0876, 
        0x2f61, 0x0e34, 0x2a6f, 0x0d24, 0x24e2, 0x1c4f, 0x144b, 0x2352, 
        0x26bf, 0x065e, 0x2b80, 0x26a2, 0x2449, 0x1a4a, 0x0f91, 0x0031, 
        0x263d, 0x05e8, 0x14f9, 0x254f, 0x2440, 0x284a, 0x230e, 0x0bd6, 
        0x0b5c, 0x0f7b, 0x07a2, 0x0f97, 0x08e9, 0x0ebc, 0x1535, 0x0127, 
        0x2051, 0x0145, 0x1766, 0x0d01, 0x1749, 0x1817, 0x0550, 0x2012, 
        0x07a6, 0x0fce, 0x1090, 0x099b, 0x29e3, 0x23ce, 0x1eb0, 0x1b88, 
        0x1aa5, 0x13c1, 0x2f41, 0x02a5, 0x052b, 0x062b, 0x0f75, 0x2fc7, 
        0x07a4, 0x0d95, 0x24e5, 0x242c, 0x1f8c, 0x2e5c, 0x21f1, 0x1e8e, 
        0x1161, 0x17e6, 0x0f14, 0x13d7, 0x1f1d, 0x2b03, 0x2247, 0x028e, 
        0x1a68, 0x17f8, 0x1354, 0x14db, 0x01d4, 0x2810, 0x2ab2, 0x0007, 
        0x0c52, 0x00d8, 0x02ff, 0x0c30, 0x209c, 0x0c9d, 0x0502, 0x0f68, 
        0x087b, 0x2480, 0x2361, 0x2484, 0x0efd, 0x0fd2, 0x2c2d, 0x2274, 
        0x1255, 0x070a, 0x1ec6, 0x2b01, 0x2c79, 0x0371, 0x1555, 0x124c, 
        0x1c86, 0x0242, 0x1dcc, 0x15f2, 0x13b3, 0x0bf9, 0x18f5, 0x11a6, 
        0x0aaa, 0x09ae, 0x290a, 0x1bcf, 0x00bd, 0x2a07, 0x16c8, 0x1b66, 
        0x07f3, 0x1d5f, 0x19d8, 0x12e2, 0x1ff0, 0x0d7b, 0x196c, 0x0b39, 
        0x2ba1, 0x036a, 0x0903, 0x2bfb, 0x1229, 0x0625, 0x2e0b, 0x0739, 
        0x1f34, 0x2c92, 0x107a, 0x1e69, 0x0dfa, 0x2803, 0x0cf5, 0x0157, 
        0x1ba6, 0x2958, 0x02cc, 0x1524, 0x0dbb, 0x2a01, 0x055d, 0x22d9, 
        0x1f83, 0x0c5b, 0x056d, 0x0d1f, 0x0e5e, 0x0722, 0x0470, 0x0811, 
        0x2233, 0x08e3, 0x13c7, 0x2b06, 0x12fc, 0x189e, 0x2530, 0x207a, 
        0x0589, 0x0ea0, 0x13ee, 0x133c, 0x052f, 0x0a9d, 0x16cc, 0x00b4, 
        0x2a80, 0x2a45, 0x2ac1, 0x1283, 0x242d, 0x2b2d, 0x0c31, 0x2e6b, 
        0x057b, 0x2f12, 0x123e, 0x0d2f, 0x1cd0, 0x247e, 0x2d93, 0x15de, 
        0x19a5, 0x1747, 0x098a, 0x2adf, 0x19c7, 0x0d0f, 0x2fed, 0x11e2, 
        0x28d5, 0x17c5, 0x274a, 0x01fa, 0x0ccc, 0x286b, 0x0ad8, 0x0000
};

const uint32_t iGMb[] = {
        0x0813, 0x248f, 0x10ee, 0x155d, 0x249f, 0x055c, 0x19db, 0x016b, 
        0x0568, 0x2b62, 0x01f4, 0x047b, 0x28c1, 0x0297, 0x018b, 0x0a7f, 
        0x2b53, 0x21e6, 0x0cbd, 0x1cef, 0x21bf, 0x2ef1, 0x17fc, 0x238c, 
        0x0ad3, 0x2044, 0x1f3e, 0x1c15, 0x11ac, 0x2cb1, 0x233e, 0x2f13, 
        0x2cfd, 0x1787, 0x0620, 0x07e8, 0x29c8, 0x2926, 0x1dcd, 0x2d1b, 
        0x12d6, 0x2777, 0x1e64, 0x1b79, 0x2ab9, 0x0f56, 0x219c, 0x2c45, 
        0x135a, 0x29a9, 0x2113, 0x04bb, 0x122b, 0x19b6, 0x085e, 0x1911, 
        0x2018, 0x02d0, 0x09ee, 0x1a0b, 0x24e0, 0x28a0, 0x1a16, 0x0273, 
        0x2c40, 0x00bf, 0x0c1a, 0x0360, 0x1f1e, 0x001c, 0x141f, 0x01fd, 
        0x0ae3, 0x0d9f, 0x0105, 0x020d, 0x1d3e, 0x1f67, 0x0eba, 0x0908, 
        0x2427, 0x192f, 0x2482, 0x27c7, 0x1a29, 0x0bbc, 0x19f6, 0x1c28, 
        0x2478, 0x29ce, 0x0684, 0x1c01, 0x1ebb, 0x1677, 0x1e1c, 0x2245, 
        0x0ab7, 0x1ce4, 0x0020, 0x19a2, 0x156d, 0x2bf2, 0x1724, 0x0da8, 
        0x0258, 0x2ce4, 0x2abf, 0x1fe9, 0x01da, 0x1db4, 0x2ebb, 0x157a, 
        0x18e3, 0x0d96, 0x2fb3, 0x1b87, 0x1cc8, 0x1ee5, 0x1b9a, 0x26b8, 
        0x124a, 0x1697, 0x12d1, 0x0f3a, 0x0d7d, 0x0b9b, 0x151b, 0x1125, 
        0x15ba, 0x0539, 0x24b5, 0x17a0, 0x19ce, 0x00c4, 0x2cd7, 0x0deb, 
        0x1c34, 0x2f58, 0x0723, 0x0e5b, 0x0cae, 0x1bcd, 0x0714, 0x0f37, 
        0x0d0c, 0x2046, 0x0f89, 0x266c, 0x271c, 0x2223, 0x25b7, 0x0514, 
        0x0f43, 0x049c, 0x2d9c, 0x0403, 0x1719, 0x0d3e, 0x12c0, 0x2fdf, 
        0x1b00, 0x0a38, 0x00e0, 0x236b, 0x05f8, 0x1398, 0x11f9, 0x2f97, 
        0x1068, 0x1a36, 0x0b33, 0x1f5b, 0x0cf6, 0x0fe8, 0x2717, 0x0653, 
        0x1e32, 0x2f19, 0x2ddf, 0x00ad, 0x0974, 0x183f, 0x0132, 0x1f03, 
        0x2004, 0x0e1e, 0x23b5, 0x0a94, 0x2e6a, 0x213c, 0x03ba, 0x297f, 
        0x1ae5, 0x14ae, 0x2ae0, 0x0757, 0x0472, 0x0004, 0x1097, 0x1bb7, 
        0x23d8, 0x243c, 0x14b8, 0x004b, 0x0b09, 0x0b58, 0x2b40, 0x15dd, 
        0x2774, 0x0399, 0x2781, 0x211d, 0x1f2b, 0x23f7, 0x0a91, 0x1f74, 
        0x20a4, 0x13b0, 0x1581, 0x264a, 0x121b, 0x2c5b, 0x2697, 0x1978, 
        0x0f3f, 0x2d46, 0x292a, 0x0a85, 0x2c35, 0x2891, 0x17e1, 0x08cf, 
        0x1bc4, 0x21d8, 0x1aae, 0x048f, 0x1bb2, 0x18d1, 0x06ad, 0x09ed, 
        0x25d8, 0x0fa8, 0x0dac, 0x1f5d, 0x2d42, 0x1221, 0x0acd, 0x0588, 
        0x1054, 0x2584, 0x24fa, 0x0fe4, 0x1680, 0x0884, 0x10bb, 0x0000
};


/* ===================================================================== */
void poly_init_q() {
    const uint32_t q = Q;
    const uint32_t mu[2] = {0x55538, 0};
    const uint32_t inv2 = 0x1801;
    ntt_lite_load_q(q, mu, 8, 14, inv2, NTT_LITE_MODE_DUAL); //9 is not supported so assign 8 and use 32 bit 256 so which makes 512 16 when use it as dual mode
}

void poly_init_ntt() {
    ntt_lite_load_twiddle((uint32_t*) GMb);
}


void poly_init_invntt() {
    ntt_lite_load_twiddle((uint32_t*) iGMb);
}
/*
 * Pack Falcon 16-bit polynomial coefficients into hardware 32-bit format.
 * Two 16-bit values are packed into each 32-bit word: [high|low]
 * 
 * falcon[0], falcon[1] -> hw[0] = (falcon[1] << 16) | falcon[0]
 * falcon[2], falcon[3] -> hw[1] = (falcon[3] << 16) | falcon[2]
 * ...
 * 
 * This allows processing 512 Falcon coefficients as 256 HW values.
 */
void
falcon_to_hw_format(uint32_t *dst, const uint16_t *src, size_t n)
{
	size_t i;
	/* n is the number of 16-bit Falcon coefficients (should be 512)
	 * We pack them into n/2 32-bit hardware values (256)
	 */
	for (i = 0; i < n / 2; i++) {
		dst[i] = ((uint32_t)src[2*i + 1] << 16) | (uint32_t)src[2*i];
	}
}

/*
 * Unpack hardware 32-bit format back to Falcon 16-bit polynomial coefficients.
 * Each 32-bit word contains two 16-bit values: [high|low]
 */
void
hw_to_falcon_format(uint16_t *dst, const uint32_t *src, size_t n)
{
	size_t i;
	/* n is the number of 16-bit Falcon coefficients (should be 512)
	 * We unpack from n/2 32-bit hardware values (256)
	 */
	for (i = 0; i < n / 2; i++) {
		dst[2*i]     = (uint16_t)(src[i] & 0xFFFF);        // Low 16 bits
		dst[2*i + 1] = (uint16_t)((src[i] >> 16) & 0xFFFF); // High 16 bits
	}
}

/*
 * Reduce a small signed integer modulo q. The source integer MUST
 * be between -q/2 and +q/2.
 */
static inline uint32_t
mq_conv_small(int x)
{
	/*
	 * If x < 0, the cast to uint32_t will set the high bit to 1.
	 */
	uint32_t y;

	y = (uint32_t)x;
	y += Q & -(y >> 31);
	return y;
}

/*
 * Addition modulo q. Operands must be in the 0..q-1 range.
 */
static inline uint32_t
mq_add(uint32_t x, uint32_t y)
{
	/*
	 * We compute x + y - q. If the result is negative, then the
	 * high bit will be set, and 'd >> 31' will be equal to 1;
	 * thus '-(d >> 31)' will be an all-one pattern. Otherwise,
	 * it will be an all-zero pattern. In other words, this
	 * implements a conditional addition of q.
	 */
	uint32_t d;

	d = x + y - Q;
	d += Q & -(d >> 31);
	return d;
}

/*
 * Subtraction modulo q. Operands must be in the 0..q-1 range.
 */
static inline uint32_t
mq_sub(uint32_t x, uint32_t y)
{
	/*
	 * As in mq_add(), we use a conditional addition to ensure the
	 * result is in the 0..q-1 range.
	 */
	uint32_t d;

	d = x - y;
	d += Q & -(d >> 31);
	return d;
}

/*
 * Division by 2 modulo q. Operand must be in the 0..q-1 range.
 */
static inline uint32_t
mq_rshift1(uint32_t x)
{
	x += Q & -(x & 1);
	return (x >> 1);
}

/*
 * Montgomery multiplication modulo q. If we set R = 2^16 mod q, then
 * this function computes: x * y / R mod q
 * Operands must be in the 0..q-1 range.
 */
static inline uint32_t
mq_montymul(uint32_t x, uint32_t y)
{
	uint32_t z, w;

	/*
	 * We compute x*y + k*q with a value of k chosen so that the 16
	 * low bits of the result are 0. We can then shift the value.
	 * After the shift, result may still be larger than q, but it
	 * will be lower than 2*q, so a conditional subtraction works.
	 */

	z = x * y;
	w = ((z * Q0I) & 0xFFFF) * Q;

	/*
	 * When adding z and w, the result will have its low 16 bits
	 * equal to 0. Since x, y and z are lower than q, the sum will
	 * be no more than (2^15 - 1) * q + (q - 1)^2, which will
	 * fit on 29 bits.
	 */
	z = (z + w) >> 16;

	/*
	 * After the shift, analysis shows that the value will be less
	 * than 2q. We do a subtraction then conditional subtraction to
	 * ensure the result is in the expected range.
	 */
	z -= Q;
	z += Q & -(z >> 31);
	return z;
}

/*
 * Montgomery squaring (computes (x^2)/R).
 */
static inline uint32_t
mq_montysqr(uint32_t x)
{
	return mq_montymul(x, x);
}

/*
 * Divide x by y modulo q = 12289.
 */
static inline uint32_t
mq_div_12289(uint32_t x, uint32_t y)
{
	/*
	 * We invert y by computing y^(q-2) mod q.
	 *
	 * We use the following addition chain for exponent e = 12287:
	 *
	 *   e0 = 1
	 *   e1 = 2 * e0 = 2
	 *   e2 = e1 + e0 = 3
	 *   e3 = e2 + e1 = 5
	 *   e4 = 2 * e3 = 10
	 *   e5 = 2 * e4 = 20
	 *   e6 = 2 * e5 = 40
	 *   e7 = 2 * e6 = 80
	 *   e8 = 2 * e7 = 160
	 *   e9 = e8 + e2 = 163
	 *   e10 = e9 + e8 = 323
	 *   e11 = 2 * e10 = 646
	 *   e12 = 2 * e11 = 1292
	 *   e13 = e12 + e9 = 1455
	 *   e14 = 2 * e13 = 2910
	 *   e15 = 2 * e14 = 5820
	 *   e16 = e15 + e10 = 6143
	 *   e17 = 2 * e16 = 12286
	 *   e18 = e17 + e0 = 12287
	 *
	 * Additions on exponents are converted to Montgomery
	 * multiplications. We define all intermediate results as so
	 * many local variables, and let the C compiler work out which
	 * must be kept around.
	 */
	uint32_t y0, y1, y2, y3, y4, y5, y6, y7, y8, y9;
	uint32_t y10, y11, y12, y13, y14, y15, y16, y17, y18;

	y0 = mq_montymul(y, R2);
	y1 = mq_montysqr(y0);
	y2 = mq_montymul(y1, y0);
	y3 = mq_montymul(y2, y1);
	y4 = mq_montysqr(y3);
	y5 = mq_montysqr(y4);
	y6 = mq_montysqr(y5);
	y7 = mq_montysqr(y6);
	y8 = mq_montysqr(y7);
	y9 = mq_montymul(y8, y2);
	y10 = mq_montymul(y9, y8);
	y11 = mq_montysqr(y10);
	y12 = mq_montysqr(y11);
	y13 = mq_montymul(y12, y9);
	y14 = mq_montysqr(y13);
	y15 = mq_montysqr(y14);
	y16 = mq_montymul(y15, y10);
	y17 = mq_montysqr(y16);
	y18 = mq_montymul(y17, y0);

	/*
	 * Final multiplication with x, which is not in Montgomery
	 * representation, computes the correct division result.
	 */
	return mq_montymul(y18, x);
}


// HW-accelerated NTT (no Montgomery)
void
mq_NTT(uint16_t *a, unsigned logn)
{
	uint32_t temp_hw[256];
	
	poly_init_ntt();
	falcon_to_hw_format(temp_hw, a, 512);
	ntt_lite_forward_ntt(temp_hw, temp_hw);
	hw_to_falcon_format(a, temp_hw, 512);
}
/*
 * Compute NTT on a ring element.
 */
void
mq_NTT_sw(uint16_t *a, unsigned logn)
{
	size_t n, t, m;

	n = (size_t)1 << logn;
	t = n;
	for (m = 1; m < n; m <<= 1) {
		size_t ht, i, j1;

		ht = t >> 1;
		for (i = 0, j1 = 0; i < m; i ++, j1 += t) {
			size_t j, j2;
			uint32_t s;

			s = GMb[m + i];
			j2 = j1 + ht;
			for (j = j1; j < j2; j ++) {
				uint32_t u, v;

				u = a[j];
				v = mq_montymul(a[j + ht], s);
				a[j] = (uint16_t)mq_add(u, v);
				a[j + ht] = (uint16_t)mq_sub(u, v);
			}
		}
		t = ht;
	}
}
// HW-accelerated iNTT (no Montgomery)
void
mq_iNTT(uint16_t *a, unsigned logn)
{
	uint32_t temp_hw[256];
	
	poly_init_invntt();
	falcon_to_hw_format(temp_hw, a, 512);
	ntt_lite_backward_ntt(temp_hw, temp_hw);
	hw_to_falcon_format(a, temp_hw, 512);
}

// SOFTWARE Montgomery conversion (keep original)
void
mq_poly_tomonty(uint16_t *f, unsigned logn)
{
	size_t u, n;

	n = (size_t)1 << logn;
	for (u = 0; u < n; u ++) {
		f[u] = (uint16_t)mq_montymul(f[u], R2);
	}
}

// HW-accelerated pointwise multiplication
void
mq_poly_montymul_ntt(uint16_t *f, const uint16_t *g, unsigned logn)
{
	uint32_t temp_f[256];
	uint32_t temp_g[256];
	
	falcon_to_hw_format(temp_f, f, 512);
	falcon_to_hw_format(temp_g, g, 512);
	ntt_lite_pwm(temp_f, temp_f, temp_g);
	hw_to_falcon_format(f, temp_f, 512);
}

// HW-accelerated subtraction
void
mq_poly_sub(uint16_t *f, const uint16_t *g, unsigned logn)
{
	uint32_t temp_f[256];
	uint32_t temp_g[256];
	
	falcon_to_hw_format(temp_f, f, 512);
	falcon_to_hw_format(temp_g, g, 512);
	ntt_lite_sub(temp_f, temp_f, temp_g);
	hw_to_falcon_format(f, temp_f, 512);
}

// Combined function: HW NTT + SW Montgomery
void
Zf(to_ntt_monty)(uint16_t *h, unsigned logn)
{
	mq_NTT(h, logn);           // HW-accelerated
	mq_poly_tomonty(h, logn);  // Software Montgomery conversion
}

/* see inner.h */
int
Zf(verify_raw)(const uint16_t *c0, const int16_t *s2,
	const uint16_t *h, unsigned logn, uint8_t *tmp)
{
	size_t u, n;
	uint16_t *tt;

	n = (size_t)1 << logn;
	tt = (uint16_t *)tmp;

	/*
	 * Reduce s2 elements modulo q ([0..q-1] range).
	 */
	for (u = 0; u < n; u ++) {
		uint32_t w;

		w = (uint32_t)s2[u];
		w += Q & -(w >> 31);
		tt[u] = (uint16_t)w;
	}

	/*
	 * Compute -s1 = s2*h - c0 mod phi mod q (in tt[]).
	 */
	mq_NTT(tt, logn);
	mq_poly_montymul_ntt(tt, h, logn);
	mq_iNTT(tt, logn);
	mq_poly_sub(tt, c0, logn);

	/*
	 * Normalize -s1 elements into the [-q/2..q/2] range.
	 */
	for (u = 0; u < n; u ++) {
		int32_t w;

		w = (int32_t)tt[u];
		w -= (int32_t)(Q & -(((Q >> 1) - (uint32_t)w) >> 31));
		((int16_t *)tt)[u] = (int16_t)w;
	}

	/*
	 * Signature is valid if and only if the aggregate (-s1,s2) vector
	 * is short enough.
	 */
	return Zf(is_short)((int16_t *)tt, s2, logn);
}

/* see inner.h */
int
Zf(compute_public)(uint16_t *h,
	const int8_t *f, const int8_t *g, unsigned logn, uint8_t *tmp)
{
	size_t u, n;
	uint16_t *tt;
	
	n = (size_t)1 << logn;
	tt = (uint16_t *)tmp;
	for (u = 0; u < n; u ++) {
		tt[u] = (uint16_t)mq_conv_small(f[u]);
		h[u] = (uint16_t)mq_conv_small(g[u]);
	}
	
	poly_init_ntt();
	mq_NTT(h, logn);
	mq_NTT(tt, logn);
	for (u = 0; u < n; u ++) {
		if (tt[u] == 0) {
			return 0;
		}
		h[u] = (uint16_t)mq_div_12289(h[u], tt[u]);
	}
	poly_init_invntt();
	mq_iNTT(h, logn);
	return 1;
}

/* see inner.h */
int
Zf(complete_private)(int8_t *G,
    const int8_t *f, const int8_t *g, const int8_t *F,
    unsigned logn, uint8_t *tmp)
{
    size_t u, n;
    uint16_t *t1, *t2;

    print_string("\n[DEBUG] Starting complete_private...\n");

    n = (size_t)1 << logn;
    t1 = (uint16_t *)tmp;
    t2 = t1 + n;

    // 1. Check Input f
    print_string("[DEBUG] Input f[0..7]: ");
    for(int i=0; i<8; i++) {
        print_u32((uint32_t)f[i]); // Cast to see raw value
        print_string(" ");
    }
    print_string("\n");

    for (u = 0; u < n; u ++) {
        t1[u] = (uint16_t)mq_conv_small(g[u]);
        t2[u] = (uint16_t)mq_conv_small(F[u]);
    }

    poly_init_ntt(); // Ensure HW tables loaded
    
    // Transform g and F
    mq_NTT(t1, logn);
    mq_NTT(t2, logn);
    
    // Compute g * F in t1
    mq_poly_tomonty(t1, logn);
    mq_poly_montymul_ntt(t1, t2, logn);

    // Load f into t2
    for (u = 0; u < n; u ++) {
        t2[u] = (uint16_t)mq_conv_small(f[u]);
    }

    // Transform f -> NTT(f)
    print_string("[DEBUG] Running NTT on f...\n");
    mq_NTT(t2, logn);

    // 2. Check NTT(f) output
    print_string("[DEBUG] NTT(f) [0..7]: ");
    for(int i=0; i<8; i++) {
        print_u32((uint32_t)t2[i]);
        print_string(" ");
    }
    print_string("\n");

    // 3. Check for zeros and divide
    int zero_count = 0;
    for (u = 0; u < n; u ++) {
        if (t2[u] == 0) {
            if (zero_count < 5) { // Only print first 5 errors
                print_string("[ERROR] f is 0 at index: ");
                print_u32((uint32_t)u);
                print_string("\n");
            }
            zero_count++;
            // Don't return yet, let's see how many
        } else {
            t1[u] = (uint16_t)mq_div_12289(t1[u], t2[u]);
        }
    }

    if (zero_count > 0) {
        print_string("[FAIL] Total zeros in NTT(f): ");
        print_u32((uint32_t)zero_count);
        print_string("\n");
        return 0;
    }

    poly_init_invntt(); // Ensure HW tables loaded
    mq_iNTT(t1, logn);

    for (u = 0; u < n; u ++) {
        uint32_t w;
        int32_t gi;

        w = t1[u];
        w -= (Q & ~-((w - (Q >> 1)) >> 31));
        gi = *(int32_t *)&w;
        if (gi < -127 || gi > +127) {
            print_string("[ERROR] G value out of range at index ");
            print_u32(u);
            print_string(": ");
            print_u32(w);
            print_string("\n");
            return 0;
        }
        G[u] = (int8_t)gi;
    }
    
    print_string("[DEBUG] complete_private success!\n");
    return 1;
}

/* see inner.h */
int
Zf(is_invertible)(
	const int16_t *s2, unsigned logn, uint8_t *tmp)
{
	size_t u, n;
	uint16_t *tt;
	uint32_t r;

	n = (size_t)1 << logn;
	tt = (uint16_t *)tmp;
	for (u = 0; u < n; u ++) {
		uint32_t w;

		w = (uint32_t)s2[u];
		w += Q & -(w >> 31);
		tt[u] = (uint16_t)w;
	}
	poly_init_ntt();
	mq_NTT(tt, logn);
	r = 0;
	for (u = 0; u < n; u ++) {
		r |= (uint32_t)(tt[u] - 1);
	}
	return (int)(1u - (r >> 31));
}

/* see inner.h */
int
Zf(verify_recover)(uint16_t *h,
	const uint16_t *c0, const int16_t *s1, const int16_t *s2,
	unsigned logn, uint8_t *tmp)
{
	size_t u, n;
	uint16_t *tt;
	uint32_t r;

	n = (size_t)1 << logn;

	/*
	 * Reduce elements of s1 and s2 modulo q; then write s2 into tt[]
	 * and c0 - s1 into h[].
	 */
	tt = (uint16_t *)tmp;
	for (u = 0; u < n; u ++) {
		uint32_t w;

		w = (uint32_t)s2[u];
		w += Q & -(w >> 31);
		tt[u] = (uint16_t)w;

		w = (uint32_t)s1[u];
		w += Q & -(w >> 31);
		w = mq_sub(c0[u], w);
		h[u] = (uint16_t)w;
	}

	/*
	 * Compute h = (c0 - s1) / s2. If one of the coefficients of s2
	 * is zero (in NTT representation) then the operation fails. We
	 * keep that information into a flag so that we do not deviate
	 * from strict constant-time processing; if all coefficients of
	 * s2 are non-zero, then the high bit of r will be zero.
	 */
	poly_init_ntt();
	mq_NTT(tt, logn);
	mq_NTT(h, logn);
	r = 0;
	for (u = 0; u < n; u ++) {
		r |= (uint32_t)(tt[u] - 1);
		h[u] = (uint16_t)mq_div_12289(h[u], tt[u]);
	}
	poly_init_invntt();
	mq_iNTT(h, logn);

	/*
	 * Signature is acceptable if and only if it is short enough,
	 * and s2 was invertible mod phi mod q. The caller must still
	 * check that the rebuilt public key matches the expected
	 * value (e.g. through a hash).
	 */
	r = ~r & (uint32_t)-Zf(is_short)(s1, s2, logn);
	return (int)(r >> 31);
}

/* see inner.h */
int
Zf(count_nttzero)(const int16_t *sig, unsigned logn, uint8_t *tmp)
{
	uint16_t *s2;
	size_t u, n;
	uint32_t r;

	n = (size_t)1 << logn;
	s2 = (uint16_t *)tmp;
	for (u = 0; u < n; u ++) {
		uint32_t w;

		w = (uint32_t)sig[u];
		w += Q & -(w >> 31);
		s2[u] = (uint16_t)w;
	}
	poly_init_ntt();
	mq_NTT(s2, logn);
	r = 0;
	for (u = 0; u < n; u ++) {
		uint32_t w;

		w = (uint32_t)s2[u] - 1u;
		r += (w >> 31);
	}
	return (int)r;
}
