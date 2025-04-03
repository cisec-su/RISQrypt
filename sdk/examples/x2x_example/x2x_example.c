#include <stdint.h>
#include "string.h"
#include "x2x.h"
#include "uart.h"
#include "util.h"


int main() {

    uint32_t data[] = {0, 103, 2000, 3327};
    const unsigned int len = sizeof(data) / sizeof(data[0]);

    uint32_t share_in_0[len];
    uint32_t share_in_1[len];

    uint32_t share_out_0[len];
    uint32_t share_out_1[len];

    uint32_t modulus = 3329;
    uint32_t seed[2] = {1, 1};

    unsigned int i, flag;

    char hex_out[32];

    uart_transmit_string("X2X Example: A2B\n\n", 20);

    x2x_set_modulus(&modulus, X2X_MODULUS_PRIME);
    x2x_seed(seed);


    uart_transmit_string("Set Modulus Done\n\n", 18);

    for (i = 0; i < len; i++) {
        share_in_0[i] = i*100; // random
        share_in_1[i] = (modulus + data[i] - share_in_0[i]);
        if (share_in_1[i] > modulus) {
            share_in_1[i] -= modulus;
        }
    }

    x2x_a2b(share_out_1, share_out_0, share_in_1, share_in_0, len);

    uart_transmit_string("A2B Done\n\n", 18);

    byte_to_hex(hex_out, (char *)share_out_0, 16);
    uart_transmit_string(hex_out, 32);
    uart_transmit_string("\n\n", 2);
    byte_to_hex(hex_out, (char *)share_out_1, 16);
    uart_transmit_string(hex_out, 32);
    uart_transmit_string("\n\n", 2);

    flag = 0;
    for (i = 0; i < len; i++) {
        if ((share_out_0[i] ^ share_out_1[i]) != data[i]) {
            flag = 1;
        }
    }

    if (!flag) {
        uart_transmit_string("PASS\n\n", 6);
    } else {
        uart_transmit_string("FAIL\n\n", 6);
    }

    return 0;
}
