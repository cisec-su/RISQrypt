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


    print_string("Keccak Example: Null digest for SHA3-256\n");

    share_in_0[0] = 0x06;

    keccak_init(17, 1);

    print_string("Init Done\n");
    print_hex((uint8_t*) share_in_0, sizeof(share_in_0), 0);
    print_string("\n");

    keccak_absorb(share_in_0, share_in_1, 30);

    print_string("Absorb Done\n");

    keccak_finish(0x00);

    print_string("Finish Done\n");

    keccak_squeeze(share_out_0, share_out_1, 8);

    print_string("Squeeze Done\n");

    print_string("Output: ");
    print_hex((uint8_t*) share_out_0, sizeof(share_out_0), 0);
    print_hex((uint8_t*) share_out_1, sizeof(share_out_1), 0);
    print_string("\n");


    if (memcmp(share_out_0, hash, 8) == 0) {
        print_string("PASS\n");
    } else {
        print_string("FAIL\n");
    }

    return 0;
}
