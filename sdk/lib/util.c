#include "util.h"
#include "uart.h"


const char LUT[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};


void byte_to_hex(char *hex, const uint8_t *byte_arr, size_t len, int reverse) {
    uint8_t byte;
    size_t i;
    for (i = 0; i < len; i++) {
        // Convert each character to hex
        byte = (uint8_t)*byte_arr;
        if (!reverse) {
            hex[i << 1] = LUT[byte >> 4];
            hex[(i << 1) + 1] = LUT[byte & 0xF];
        } else {
            hex[(len - i - 1) << 1] = LUT[byte >> 4];
            hex[((len - i - 1) << 1) + 1] = LUT[byte & 0xF];       
        }
        byte_arr++;
    }
}


void print_string(const char *str) {
    uart_transmit_string(str, strlen(str));
}


void print_hex(const uint8_t *byte_arr, size_t len, unsigned int reverse) {
    size_t i;
    uint8_t hex_out[2];
    for (i = 0; i < len; i++) {
        if (reverse) {
          byte_to_hex(hex_out, byte_arr + (len - i - 1), 1, 0);
        }
        else {
          byte_to_hex(hex_out, byte_arr + i, 1, 0);
        }      
        uart_transmit_string(hex_out, 2);
    }
}


void print_u32(uint32_t num) {
    print_hex((const uint8_t*) &num, sizeof(uint32_t), 1);
}