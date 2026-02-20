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

    print_string("X2X Example: A2B\n");

    x2x_set_modulus(modulus, 12, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_EN);
    x2x_seed(seed);


    print_string("Set Modulus Done\n");

    for (i = 0; i < len; i++) {
        share_in_0[i] = i*100; // random
        share_in_1[i] = (modulus + data[i] - share_in_0[i]);
        if (share_in_1[i] > modulus) {
            share_in_1[i] -= modulus;
        }
    }

    x2x_a2b(share_out_1, share_out_0, share_in_1, share_in_0, len);

    print_string("A2B Done\n");
    print_u32(share_out_0[0]);
    print_string("\n");
    print_u32(share_out_1[0]);
    print_string("\n");

    flag = 0;
    for (i = 0; i < len; i++) {
        if ((share_out_0[i] ^ share_out_1[i]) != data[i]) {
            flag = 1;
        }
    }

    if (!flag) {
        print_string("PASS\n");
    } else {
        print_string("FAIL\n");
    }

    return 0;
}
