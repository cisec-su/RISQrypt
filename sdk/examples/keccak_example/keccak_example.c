#include <stdint.h>
#include "string.h"
#include "keccak.h"
#include "uart.h"
#include "util.h"


int main() {

    uint32_t share_in_0[30] = {0x00};
    uint32_t share_in_1[30] = {0x00};

    uint32_t share_out_0[8];
    uint32_t share_out_1[8];

    char hex_out[64];

    uint32_t hash[8] = {0xf8c6ffa7, 0x66d71ebf, 0x5647c151, 0x62d661a0, 0x4dff80f5, 0xfa493be4, 0x4b0ad882, 0x4a43f880};


    uart_transmit_string("Keccak Example: Null digest for SHA3-256\n\n", 41);

    share_in_0[0] = 0x06;

    keccak_init(17, 1);

    uart_transmit_string("Init Done\n\n", 11);

    byte_to_hex(hex_out, (char *)share_in_0, 32, 0);
    uart_transmit_string(hex_out, 64);
    uart_transmit_string("\n\n", 2);

    keccak_absorb(share_in_0, share_in_1, 30);

    uart_transmit_string("Absorb Done\n\n", 13);

    keccak_finish(0x00);

    uart_transmit_string("Finish Done\n\n", 13);

    keccak_squeeze(share_out_0, share_out_1, 8);

    uart_transmit_string("Squeeze Done\n\n", 14);

    uart_transmit_string("Output: ", 8);
    if (share_out_0[0] == 0x00000000 && share_out_1[0] == 0x00000000) {
        uart_transmit_string("Digest is null\n\n", 16);
    }
    byte_to_hex(hex_out, (char *)share_out_0, 32, 0);
    uart_transmit_string(hex_out, 64);
    uart_transmit_string("\n\n", 2);
    byte_to_hex(hex_out, (char *)share_out_1, 32, 0);
    uart_transmit_string(hex_out, 64);
    uart_transmit_string("\n\n", 2);


    if (memcmp(share_out_0, hash, 8) == 0) {
        uart_transmit_string("PASS\n\n", 6);
    } else {
        uart_transmit_string("FAIL\n\n", 6);
    }

    return 0;
}
