#include <stdint.h>
#include "string.h"
#include "ntt_lite.h"
#include "uart.h"
#include "util.h"


int main () {

    size_t i;
	const unsigned int logn = 4;
	const unsigned int logq = 64;

	uint64_t q = 0x8000000000080001;
	uint64_t w[15] = {0x5ae9e1854717f387, 0x52ef42e4720ecf90, 0x2f14ffb3578432ed, 0x5801325ccbcc0254, 0x567c412e5d767830, 0x2f9b342c718f9727, 0x764c4b10ee05fa6a, 0x51fe10ef0f47f7f1, 0x2de1635d3f7d0b79, 0xde8083811ed7498, 0x479017e9b0d5c844, 0x15e0b3c5e9155e90, 0x63322991fbae4d17, 0x68d02859dafe34ca, 0x26e73520e16d7a50};
    uint64_t a[16] = {0x7c65c1e582e2e662, 0x5ba91faf7a024204, 0x37ebdcd9e87a1613, 0x23a7711a81332876, 0x23c6612f48268673, 0x1846d424c17c6279, 0x259f4329e6f4590b, 0x19488dec4f65d4d9, 0x12e0c8b2bad640fb, 0x5487ce1eaf19922a, 0x5a92118719c78df4, 0x50f244556f25e2a2, 0x3458a748e9bb17bc, 0x71545a137a1d5006, 0xff18e0242af9fc3, 0x17e0aa3c03983ca8};
	uint64_t a_ntt[16] = {0x44ad954751ec4a86, 0x7f5b672601747271, 0x794620f9de14ca67, 0x66376473c992cb6a, 0x56b699859ed44766, 0x1527bf3c8a0d5ea, 0x5f102109b07437c, 0x11694cd5826ac17c, 0x2b294fbbaf0a4871, 0x778526bf372467cf, 0x1509cadd23931a97, 0x5ce6cb8e7685e, 0x71d82a36b990a18a, 0x6174d29d4c397f83, 0x23c5b0cf9906af19, 0x4a17ab54c84edad};
	uint64_t b[16];
    char hex_out[sizeof(uint64_t) << 1];

    uart_transmit_string("NTT Example: N=16 Forward NTT\n\n", 31);

	ntt_lite_forward((uint32_t*) b, (uint32_t*) a, (uint32_t*) w, (uint32_t*) &q, logn, logq);

    uart_transmit_string("NTT Done\n\n", 10);

    uart_transmit_string("Output: ", 8);
    for (i = 0; i < 16; i++) {
        byte_to_hex(hex_out, (const char *)(b + i), sizeof(uint64_t));
        uart_transmit_string(hex_out, sizeof(uint64_t) << 1);
        uart_transmit_string("\n", 1);
    }
    uart_transmit_string("\n", 1);

	if (memcmp(a_ntt, b, sizeof(uint64_t)*(1 << logn)) == 0) {
        uart_transmit_string("PASS\n\n", 6);
    } else {
        uart_transmit_string("FAIL\n\n", 6);
    }

	return 0;
}
