#include "util.h"

char LUT[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void byte_to_hex(char *hex, const char *byte_arr, size_t len, int reverse) {
    unsigned char byte;
    size_t i = 0;
    for (i = 0; i < len; i++) {
        // Convert each character to hex
        byte = (unsigned char)*byte_arr;
        if (!reverse) {
        } else {
            hex[(len - i - 1) << 1] = LUT[byte >> 4];
            hex[((len - i - 1) << 1) + 1] = LUT[byte & 0xF];       
        }
        byte_arr++;
    }
}
