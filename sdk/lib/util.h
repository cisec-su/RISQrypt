#ifndef UTIL_H_
#define UTIL_H_


#include <stdlib.h>
#include <stdint.h>
#include <string.h>


void byte_to_hex(char *hex, const uint8_t *byte_arr, size_t len, int reverse);

void print_string(const char *str);

void print_hex(const uint8_t *byte_arr, size_t len, unsigned int reverse);

void print_u32(uint32_t num);

void print_u32_arr(uint32_t *ptr, size_t len);


#endif