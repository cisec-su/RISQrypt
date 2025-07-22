#include <stdint.h>
#include "string.h"
#include "x2x.h"
#include "uart.h"
#include "util.h"


const unsigned int len = 256;
const unsigned int len_d = 128;
const unsigned int len_bit = 8;

uint32_t share_A_s_0[256];
uint32_t share_A_s_1[256];

uint32_t share_A_d_0[128];
uint32_t share_A_d_1[128];

uint32_t share_B_s_0_1bit[] = {0x7490945, 0x20802a2, 0x5ad0c5a, 0xb6d01fc, 0x56e01be, 0x30f0795, 0xc330465, 0x19a07fb};
uint32_t share_B_s_1_1bit[] = {0x7640431, 0x5c09c7, 0xd290b44, 0xe4f0f5d, 0xd8b05f8, 0x1300fb0, 0xcc10611, 0xeae0421};

uint32_t test_B2A_single_q_1bit(uint32_t modulus_q, uint32_t log_stride)
{
    uint32_t stride = 1 << log_stride;
    x2x_b2a_1bit(share_A_s_1, share_A_s_0, share_B_s_1_1bit, share_B_s_0_1bit, log_stride, len_bit);
    int test = 1;
    
    for (unsigned int i = 0; i < stride; i++)
    {
        for (unsigned int j = 0; j < (len >> log_stride); j++)
        {
            uint32_t element_index, bit_index, array_in_index, array_out_index;
            element_index = (j << log_stride) + i;
            bit_index = element_index & 0x1f;
            array_in_index = element_index >> 5;
            array_out_index = (i*(len >> log_stride)) + j;
            
            uint32_t s_in_0 = (share_B_s_0_1bit[array_in_index]>>(bit_index))&0x1;
            uint32_t s_in_1 = (share_B_s_1_1bit[array_in_index]>>(bit_index))&0x1;
            uint32_t s_out_0 = share_A_s_0[array_out_index];
            uint32_t s_out_1 = share_A_s_1[array_out_index];

            if((s_in_0 ^ s_in_1) != ((s_out_0 + s_out_1) % modulus_q))
            {
                uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
                print_u32(i);uart_transmit_string("\n\n", 2);
                print_u32(j);uart_transmit_string("\n\n", 2);
                print_u32(s_in_0);uart_transmit_string("\n", 1);
                print_u32(s_in_1);uart_transmit_string("\n", 1);
                print_u32(s_out_0);uart_transmit_string("\n", 1);
                print_u32(s_out_1);uart_transmit_string("\n", 1);
                uart_transmit_string("WRONG\n\n",7);
                test = 0;
            }
        }
    }
    return test;
}

uint32_t test_B2A_dual_q_1bit(uint32_t modulus_q, uint32_t log_stride)
{
    uint32_t stride = 1 << log_stride;
    x2x_b2a_1bit(share_A_d_1, share_A_d_0, share_B_s_1_1bit, share_B_s_0_1bit, log_stride, len_bit);
    int test = 1;
    for (unsigned int i = 0; i < stride; i++)
    {
        for (unsigned int j = 0; j < (len >> log_stride); j++)
        {
            uint32_t element_index, bit_index, array_in_index, array_out_index;
            element_index = (j << log_stride) + i;
            bit_index = element_index & 0x1f;
            array_in_index = element_index >> 5;
            array_out_index = (i*(len >> log_stride)) + j;
            
            uint32_t s_in_0 = (share_B_s_0_1bit[array_in_index]>>(bit_index))&0x1;
            uint32_t s_in_1 = (share_B_s_1_1bit[array_in_index]>>(bit_index))&0x1;
            uint32_t s_out_0 = share_A_d_0[array_out_index >> 1];
            uint32_t s_out_1 = share_A_d_1[array_out_index >> 1];


            if(array_out_index % 2)
            {
                s_out_0 = s_out_0 >> 16;
                s_out_1 = s_out_1 >> 16;   
            }
            else
            {
                s_out_0 = s_out_0 & 0xffff;
                s_out_1 = s_out_1 & 0xffff;
            }

            if((s_in_0 ^ s_in_1) != ((s_out_0 + s_out_1) % modulus_q))
            {
                uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
                print_u32(i);uart_transmit_string("\n\n", 2);
                print_u32(j);uart_transmit_string("\n\n", 2);
                print_u32(s_in_0);uart_transmit_string("\n", 1);
                print_u32(s_in_1);uart_transmit_string("\n", 1);
                print_u32(s_out_0);uart_transmit_string("\n", 1);
                print_u32(s_out_1);uart_transmit_string("\n", 1);
                uart_transmit_string("WRONG\n\n",7);
                test = 0;
            }
        }
    }
    return test;
}

int main() {
    uint32_t seed[] = {1, 1};
    x2x_seed(seed);


    ////////////////////// PRIME
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    uint32_t test;


    // SINGLE PRIME
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    test = 1;
    for (unsigned int log_stride = 0; log_stride < 8; log_stride++)
        test = test & test_B2A_single_q_1bit(modulus_q, log_stride);
    
    if(test == 1)
        uart_transmit_string("1-BIT SINGLE PRIME PASS\n\n", 25); 
    else
        uart_transmit_string("1-BIT SINGLE PRIME FAIL\n\n", 25); 

    // DUAL PRIME
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    test = 1;
    for (unsigned int log_stride = 0; log_stride < 8; log_stride++)
        test = test & test_B2A_dual_q_1bit(modulus_q, log_stride);

    if(test == 1)
        uart_transmit_string("1-BIT DUAL PRIME PASS\n\n", 23); 
    else
        uart_transmit_string("1-BIT DUAL PRIME FAIL\n\n", 23); 

    /////////// POW 2

    uint32_t modulus_2k = 0x1000;
    uint32_t log_modulus_2k = 12;

    // SINGLE POW 2
    x2x_set_modulus(modulus_2k, log_modulus_2k, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    test = 1;
    for (unsigned int log_stride = 0; log_stride < 8; log_stride++)
        test = test & test_B2A_single_q_1bit(modulus_2k, log_stride);

    if(test == 1)
        uart_transmit_string("1-BIT SINGLE POW2 PASS\n\n", 24); 
    else
        uart_transmit_string("1-BIT SINGLE POW2 FAIL\n\n", 24); 

    // DUAL POW 2
    x2x_set_modulus(modulus_2k, log_modulus_2k, X2X_MODULUS_POW2, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    test = 1;
    for (unsigned int log_stride = 0; log_stride < 8; log_stride++)
        test = test & test_B2A_dual_q_1bit(modulus_2k, log_stride);
    
    if(test == 1)
        uart_transmit_string("1-BIT DUAL POW2 PASS\n\n", 22); 
    else
        uart_transmit_string("1-BIT DUAL POW2 FAIL\n\n", 22); 

    //

    //TEST 10 B --> A (DUAL-q 1bit)
    
    //test_B2A_dual_q_1bit_str_2();
    //

    //test_B2A_single_2_1bit();
    //test_B2A_dual_2_1bit();
    
    
    return 0;
}
