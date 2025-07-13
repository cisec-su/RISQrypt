#include <stdint.h>
#include "string.h"
#include "x2x.h"
#include "uart.h"
#include "util.h"


const unsigned int len = 256;
const unsigned int len_d = 128;

uint32_t unmasked_data_s_q[] = {0x0, 0x1, 0xd00, 0xcbd, 0x6a2, 0x89e, 0x2cd, 0x135, 0xb4c, 0x741, 0xab1, 0x4e5, 0x8ec, 0x155, 0x8e3, 0x229, 0x76c, 0x38b, 0x4d3, 0x39a, 0x65d, 0x3d6, 0xced, 0xbdd, 0xabd, 0x88d, 0x656, 0xa96, 0x843, 0x257, 0x19a, 0x220, 0xb0f, 0x65e, 0x351, 0xb, 0x89e, 0x68f, 0xcd, 0xc62, 0x62d, 0x37c, 0xb50, 0x5c5, 0x83b, 0xa06, 0xa5d, 0x601, 0x24, 0x4d3, 0x10a, 0x9e, 0x902, 0x4d1, 0xc, 0x3f3, 0xbce, 0x25b, 0x8f0, 0x57a, 0xb61, 0x16e, 0x699, 0x4a8, 0xfa, 0xa29, 0x67d, 0x86b, 0x214, 0x66a, 0x2c8, 0x67a, 0xb52, 0x1fa, 0x3dc, 0x248, 0x62f, 0xa52, 0xa79, 0x16b, 0x1ae, 0x268, 0xd3, 0xc14, 0x9f6, 0x6e8, 0x436, 0x214, 0xca7, 0x1ee, 0x420, 0x82, 0xcdc, 0x3f6, 0x3ee, 0x20e, 0x1e4, 0x8ea, 0xab4, 0xbec, 0xa44, 0x588, 0x496, 0x8d5, 0x482, 0x6c4, 0x9ac, 0x720, 0x2e3, 0x811, 0x23e, 0x7b9, 0x79, 0xa69, 0xcee, 0x4f2, 0x647, 0x5e7, 0xa49, 0x590, 0x3a, 0x5d8, 0x539, 0x85c, 0x862, 0x97a, 0x4a1, 0x741, 0x81c, 0x691, 0xae5, 0x8b3, 0xde, 0x8ba, 0x9b1, 0x67a, 0x5ec, 0xa79, 0x7c5, 0x78, 0x9c1, 0xa2d, 0x4ee, 0xa4d, 0xc20, 0x3c0, 0x88b, 0x666, 0x8c3, 0xa6e, 0x8d9, 0x59b, 0x9c, 0x298, 0x86b, 0x2e4, 0x615, 0x8d8, 0x2fe, 0x805, 0x957, 0x804, 0xae3, 0xc08, 0x5af, 0x59e, 0x61c, 0xa77, 0x466, 0x2d5, 0x298, 0x278, 0x3b8, 0xbdb, 0xac4, 0x892, 0x61b, 0x6ba, 0x78f, 0x2e1, 0x7c5, 0x9d3, 0x10e, 0x24a, 0x4f, 0x5b8, 0xb3f, 0x887, 0xb0e, 0xc89, 0xcf9, 0x59d, 0x1c6, 0x83c, 0x450, 0xc16, 0x4a9, 0x6c1, 0x11c, 0xa0d, 0x748, 0xbdf, 0xce9, 0xb3f, 0x1e1, 0x37e, 0x5b6, 0xc71, 0xa12, 0x2fb, 0xcf9, 0xcf8, 0x721, 0xa37, 0x402, 0x144, 0x1b3, 0x4e3, 0x90e, 0x52d, 0x5ad, 0xab, 0xa06, 0x375, 0x151, 0xb19, 0x800, 0x179, 0x15b, 0x135, 0x4f3, 0xccf, 0xc94, 0x2f2, 0x8f6, 0x549, 0x32f, 0x8ae, 0x931, 0x9c2, 0x378, 0x5ec, 0x45, 0x8d, 0x960, 0x671, 0xa79, 0x3a0, 0x373, 0x3e6, 0x5f3, 0xb87, 0xb91, 0xc4c, 0x87f, 0xc65};
uint32_t unmasked_data_s_2[] = {0x0, 0x1, 0xfff, 0xd9a, 0x9fb, 0xd1a, 0xb2c, 0x1cc, 0xfd6, 0xbf2, 0x5f3, 0x291, 0x51d, 0x54a, 0xdc8, 0xda7, 0x9a5, 0x6b5, 0x67c, 0xf9f, 0x1b0, 0x649, 0x5bf, 0x176, 0xea8, 0xdd6, 0x12d, 0xdaa, 0x646, 0x1eb, 0x870, 0xe31, 0xd8, 0x3bc, 0xf13, 0x1d2, 0x348, 0xe9a, 0x3cb, 0xfc, 0x5c3, 0xdb, 0xba3, 0xb7d, 0x4dd, 0xcbf, 0x916, 0xf7, 0x8e4, 0x953, 0x225, 0x26d, 0xbd9, 0x27c, 0xd83, 0x5, 0x9f8, 0x3c8, 0x12b, 0xf9a, 0x89, 0x897, 0xcc0, 0xf07, 0x67b, 0x17e, 0x51e, 0x4e6, 0x169, 0x41f, 0xf06, 0x88e, 0xd43, 0x4e4, 0x296, 0x201, 0x2dc, 0xabf, 0xf96, 0x16f, 0xe63, 0x51d, 0x33a, 0xea7, 0xee8, 0x8e1, 0x388, 0x57f, 0x950, 0xeaa, 0x35f, 0x1ac, 0xbd9, 0x30d, 0xcdd, 0xffe, 0xddd, 0xb2a, 0xc6e, 0xed7, 0x8c3, 0xf62, 0x32b, 0x99d, 0xa9d, 0xd6b, 0x741, 0x371, 0xd30, 0xa82, 0x9de, 0xba8, 0x9a2, 0x3bf, 0x5ac, 0x56a, 0x185, 0xfb8, 0xa2d, 0xbdf, 0x814, 0xa92, 0x6e6, 0x385, 0xcc7, 0x3b5, 0xa95, 0x3dd, 0x2ca, 0xefc, 0x88b, 0xe8e, 0x30f, 0xc7f, 0x1, 0xa03, 0x54, 0x15d, 0xc2e, 0x489, 0xcac, 0xdda, 0xaa2, 0xe68, 0xa8d, 0x432, 0x259, 0x632, 0xcf8, 0xd7f, 0x7d7, 0x3e6, 0xe84, 0xeb0, 0xba6, 0xf7a, 0x300, 0x516, 0x34c, 0xa3a, 0x9f9, 0x105, 0xb7f, 0x84, 0x8e4, 0x4af, 0xd20, 0x9c1, 0xd64, 0xcdb, 0xbb4, 0x637, 0x9bd, 0x792, 0x2d4, 0x599, 0xa6e, 0x59d, 0x961, 0x6e9, 0x857, 0xe24, 0x9a0, 0x184, 0xb33, 0xe81, 0x9a9, 0xd26, 0x8c6, 0xc1e, 0xfdf, 0xde, 0xe6c, 0xb12, 0x767, 0x24b, 0x747, 0x2b9, 0x4d2, 0xc1d, 0xfa7, 0x9d7, 0x394, 0x581, 0x956, 0x408, 0xf3d, 0x2ff, 0xd88, 0xd2a, 0xac5, 0xa9c, 0x7e8, 0x341, 0xfb4, 0x4dc, 0xa11, 0x264, 0x9bc, 0xd7e, 0xef7, 0xc75, 0x3f2, 0x2d4, 0xeb0, 0xf89, 0xbd6, 0x6a0, 0xe17, 0xc82, 0xc09, 0x3f1, 0xea, 0x173, 0x5f8, 0x5ab, 0x422, 0xfd6, 0xbb4, 0xd12, 0xfe4, 0xbfd, 0x2a3, 0x118, 0x32e, 0xe6d, 0x30f, 0x2f4, 0xe7f, 0xec7, 0x8c1, 0x4dd, 0xe99, 0x4e, 0x614, 0x594};

uint32_t share_B_s_0[256];
uint32_t share_B_s_1[256];
uint32_t share_A_s_0[256];
uint32_t share_A_s_1[256];

uint32_t unmasked_data_d_q[] = {0x7490945, 0x20802a2, 0x5ad0c5a, 0xb6d01fc, 0x56e01be, 0x30f0795, 0xc330465, 0x19a07fb, 0x9a102d5, 0x7610bed, 0x5600462, 0x80002e7, 0x6e80cbb, 0x6fe09aa, 0x41c09ca, 0xb700962, 0x1df0114, 0x892067c, 0x3f0811, 0xcd90bd9, 0x60900fc, 0x11a05d9, 0xa09082c, 0xc9c04af, 0x31d09ed, 0x5240278, 0x7b20af0, 0xab20624, 0x6390cc6, 0x62a0a39, 0xabd0b83, 0xa2f08c6, 0xb5d061d, 0x6a700b8, 0x4b70998, 0x3970bde, 0x5c405cb, 0xc64075c, 0x9840a37, 0x4740136, 0x5f907f7, 0x1870251, 0x643038f, 0xb0d05c4, 0x8100947, 0x92f0bc5, 0xae00237, 0x42607b0, 0x44e039b, 0x20006ec, 0x2b407db, 0x48c08b6, 0xc100629, 0x1750398, 0x662015c, 0x6500c6b, 0xc5b0472, 0xb440a38, 0x3b30c85, 0x4db0976, 0x2530498, 0xa5501f3, 0x28408d7, 0xdc02fc, 0xb4012b, 0x52805fd, 0xa710154, 0xb8e008a, 0x77d0ab9, 0x52805a4, 0xb030726, 0x4b704aa, 0x404016d, 0xba09b5, 0x9a10626, 0x99c0cae, 0xa220639, 0xa42076d, 0x64902f6, 0x8730602, 0x4650835, 0x7650776, 0x42f056b, 0x1930982, 0xe009e6, 0xd401a7, 0x3a0ab0, 0x3e406b1, 0xbad01f5, 0x628012e, 0x3ea05cb, 0x5f30421, 0x58e0ae2, 0x7890087, 0x8920b92, 0x971000c, 0x3e4034a, 0x85103fb, 0x18805c3, 0x9f10b39, 0xbcb0071, 0x71203a0, 0xac60181, 0x88201ab, 0x18c05f9, 0x7100ce8, 0xca00763, 0xa320051, 0x930bdd, 0x9730cee, 0x6c7086e, 0x580366, 0x25308ba, 0x2c9075f, 0xa560793, 0x9dd0899, 0x2290072, 0x4920692, 0x27704f3, 0x55d0bf4, 0xc360933, 0x1150b01, 0x72e0341, 0x8f004b6, 0x46f0204, 0x7c10b2c, 0x2c50b9d, 0xaf00802};
uint32_t unmasked_data_d_2[] = {0xee0aa2, 0x40c0196, 0xd74019f, 0x3c70a3f, 0x740b74, 0xfbc0fb4, 0x1040635, 0x8640552, 0x2ac0cca, 0xc0c0c4d, 0xb4a0bea, 0x64603db, 0x91e066b, 0x8d4081c, 0x8ee066c, 0x7a6097f, 0xcf509c9, 0xdca09e5, 0x5d20e03, 0xe770cc3, 0x2a80755, 0xa4a0d13, 0xa8a09c5, 0x60209f6, 0x83c0336, 0x17803f2, 0x77103d6, 0x7240179, 0x8230a12, 0xda40135, 0x86b030a, 0x5600871, 0xd030d40, 0x9de0bfb, 0x9c20360, 0xf160e85, 0xcc509b6, 0x4100cb1, 0xf5003b0, 0xbc60e54, 0x8f40e89, 0x1b20ba0, 0x3b40fae, 0x85b054d, 0x2560b6e, 0xc40672, 0xa6e015a, 0xe5b00fe, 0x9c50feb, 0x934010a, 0xcd803b8, 0xe440dd6, 0x2be0f62, 0x5be04c2, 0x53b0491, 0x79a0158, 0x37e06f8, 0xc2e010c, 0x28d0625, 0xa30041e, 0x6770829, 0x7e4072b, 0x2c60b36, 0x9230a64, 0x4f1010f, 0x5660ed7, 0x2730167, 0x3180aef, 0x65000d3, 0xaf20f13, 0xa3000b3, 0xc1403c9, 0xd05050b, 0x3420387, 0xdc50446, 0x3220068, 0xcc300bb, 0xbf301e6, 0xf450220, 0x4350ed4, 0xd6f0894, 0x82304b7, 0x5100c07, 0x48e0007, 0x5eb0a93, 0xade0cb4, 0x45a08ce, 0x87d0ba4, 0x66b0a8b, 0xdcc0f27, 0xb9909b7, 0x5450cd3, 0xabf0808, 0xe5e035e, 0x7130412, 0xb650fff, 0x3eb084f, 0x4430799, 0xba30479, 0x360773, 0x2e206b7, 0x47c046a, 0x6b30f47, 0x7550894, 0x77f0cfd, 0x44904b3, 0xe10442, 0x47d0d65, 0xe5e0a00, 0x8a904cf, 0x7b903fd, 0x3bf097c, 0x65c01f9, 0xac0dfe, 0xd0b0c65, 0x9090b55, 0x34b038c, 0xc2d00d7, 0xe4c030f, 0x7640431, 0x5c09c7, 0xd290b44, 0xe4f0f5d, 0xd8b05f8, 0x1300fb0, 0xcc10611, 0xeae0421, 0x2f70dc4};


uint32_t share_B_d_0[128];
uint32_t share_B_d_1[128];
uint32_t share_A_d_0[128];
uint32_t share_A_d_1[128];

uint32_t share_B_s_0_1bit[] = {0x7490945, 0x20802a2, 0x5ad0c5a, 0xb6d01fc, 0x56e01be, 0x30f0795, 0xc330465, 0x19a07fb};
uint32_t share_B_s_1_1bit[] = {0x7640431, 0x5c09c7, 0xd290b44, 0xe4f0f5d, 0xd8b05f8, 0x1300fb0, 0xcc10611, 0xeae0421};


void test_B_share_single_q()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;

    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_b_share(share_B_s_1, share_B_s_0, unmasked_data_s_q, len);
    uart_transmit_string("B SHARE SINGLE PRIME\n\n", 22);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        uint32_t s_in = unmasked_data_s_q[i];
        if(s_in != (share_B_s_0[i]^share_B_s_1[i]))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_s_q[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 

}

void test_B2A_single_q()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_b2a(share_A_s_1, share_A_s_0, share_B_s_1, share_B_s_0, len);
    uart_transmit_string("B2A SINGLE PRIME\n\n", 18);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        
        uint32_t s_in = unmasked_data_s_q[i];
        if((s_in) != ((share_A_s_0[i]+share_A_s_1[i]) % modulus_q))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_B_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_A_share_single_q()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_a_share(share_A_s_1, share_A_s_0, unmasked_data_s_q, len);
    uart_transmit_string("A MASK SINGLE PRIME\n\n", 21);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        uint32_t s_in = unmasked_data_s_q[i];
        if(s_in != ((share_A_s_0[i]+share_A_s_1[i]) % modulus_q))
        {   
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_s_q[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 

}

void test_A2B_single_q()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_a2b(share_B_s_1, share_B_s_0, share_A_s_1, share_A_s_0, len);
    uart_transmit_string("A2B SINGLE PRIME\n\n", 18);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        uint32_t s_in = unmasked_data_s_q[i];
        if((s_in) != ((share_B_s_0[i]^share_B_s_1[i]) % modulus_q ))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(s_in);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_B_share_dual_q()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_b_share(share_B_d_1, share_B_d_0, unmasked_data_d_q, len_d);
    uart_transmit_string("B MASK DUAL PRIME\n\n", 19);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        uint32_t s_in = unmasked_data_d_q[i] & 0xffff;
        uint32_t s_out0 = share_B_d_0[i] & 0xffff;
        uint32_t s_out1 = share_B_d_1[i] & 0xffff;
        if(s_in != (s_out0^s_out1))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_q[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in = unmasked_data_d_q[i] >> 16;
        s_out0 = share_B_d_0[i] >> 16;
        s_out1 = share_B_d_1[i] >> 16;
        if(s_in != (s_out0^s_out1))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_q[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 

}

void test_B2A_dual_q()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_b2a(share_A_d_1, share_A_d_0, share_B_d_1, share_B_d_0, len_d);
    uart_transmit_string("B2A DUAL PRIME\n\n", 16);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        uint32_t s_in = unmasked_data_d_q[i] & 0xffff;
        uint32_t s_out0 = share_A_d_0[i] & 0xffff;
        uint32_t s_out1 = share_A_d_1[i] & 0xffff;
        if(s_in != ((s_out0+s_out1) % modulus_q))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_q[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in = unmasked_data_d_q[i] >> 16;
        s_out0 = share_A_d_0[i] >> 16;
        s_out1 = share_A_d_1[i] >> 16;
        if(s_in != ((s_out0+s_out1) % modulus_q))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_q[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_A_share_dual_q()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_a_share(share_A_d_1, share_A_d_0, unmasked_data_d_q, len_d);
    uart_transmit_string("A MASK DUAL PRIME\n\n", 19);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        uint32_t s_in = unmasked_data_d_q[i] & 0xffff;
        uint32_t s_out0 = share_A_d_0[i] & 0xffff;
        uint32_t s_out1 = share_A_d_1[i] & 0xffff;
        if(s_in != ((s_out0+s_out1) % modulus_q))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_q[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in = unmasked_data_d_q[i] >> 16;
        s_out0 = share_A_d_0[i] >> 16;
        s_out1 = share_A_d_1[i] >> 16;
        if(s_in != ((s_out0+s_out1) % modulus_q))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_q[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 

}

void test_A2B_dual_q()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_a2b(share_B_d_1, share_B_d_0, share_A_d_1, share_A_d_0, len_d);
    uart_transmit_string("A2B DUAL PRIME\n\n", 16);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        uint32_t s_in = unmasked_data_d_q[i] & 0xffff;
        uint32_t s_out0 = share_B_d_0[i] & 0xffff;
        uint32_t s_out1 = share_B_d_1[i] & 0xffff;
        if(s_in != ((s_out0^s_out1)%modulus_q ))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_q[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in = unmasked_data_d_q[i] >> 16;
        s_out0 = share_B_d_0[i] >> 16;
        s_out1 = share_B_d_1[i] >> 16;
        if(s_in != ((s_out0^s_out1)%modulus_q ))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(unmasked_data_d_q[i]);uart_transmit_string("\n", 1);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_B_share_single_2()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_b_share(share_B_s_1, share_B_s_0, unmasked_data_s_2, len);
    uart_transmit_string("B MASK SINGLE POW 2\n\n", 21);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        uint32_t s_in = unmasked_data_s_2[i];
        if(s_in != (share_B_s_0[i]^share_B_s_1[i]))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_s_2[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 

}

void test_B2A_single_2()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_b2a(share_A_s_1, share_A_s_0, share_B_s_1, share_B_s_0, len);
    uart_transmit_string("B2A SINGLE POW 2\n\n", 18);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        
        uint32_t s_in = unmasked_data_s_2[i];
        if(s_in != (share_A_s_0[i]+share_A_s_1[i]) % modulus_2)
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_B_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_A_share_single_2()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_a_share(share_A_s_1, share_A_s_0, unmasked_data_s_2, len);
    uart_transmit_string("A MASK SINGLE POW 2\n\n", 21);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        uint32_t s_in = unmasked_data_s_2[i];
        if(s_in != ((share_A_s_0[i]+share_A_s_1[i]) % modulus_2))
        {   
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_s_2[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 

}

void test_A2B_single_2()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_a2b(share_B_s_1, share_B_s_0, share_A_s_1, share_A_s_0, len);
    uart_transmit_string("A2B SINGLE POW 2\n\n", 18);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        uint32_t s_in = unmasked_data_s_2[i];
        if((s_in) != ((share_B_s_0[i]^share_B_s_1[i]) ))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(s_in);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_B_share_dual_2()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_b_share(share_B_d_1, share_B_d_0, unmasked_data_d_2, len_d);
    uart_transmit_string("B MASK DUAL POW 2\n\n", 19);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        uint32_t s_in = unmasked_data_d_2[i] & 0xffff;
        uint32_t s_out0 = share_B_d_0[i] & 0xffff;
        uint32_t s_out1 = share_B_d_1[i] & 0xffff;
        if(s_in != (s_out0^s_out1))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_2[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in = unmasked_data_d_2[i] >> 16;
        s_out0 = share_B_d_0[i] >> 16;
        s_out1 = share_B_d_1[i] >> 16;
        if(s_in != (s_out0^s_out1))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_2[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 

}

void test_B2A_dual_2()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_b2a(share_A_d_1, share_A_d_0, share_B_d_1, share_B_d_0, len_d);
    uart_transmit_string("B2A DUAL POW 2\n\n", 16);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        uint32_t s_in = unmasked_data_d_2[i] & 0xffff;
        uint32_t s_out0 = share_A_d_0[i] & 0xffff;
        uint32_t s_out1 = share_A_d_1[i] & 0xffff;
        if(s_in != ((s_out0+s_out1) % modulus_2))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_2[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in = unmasked_data_d_2[i] >> 16;
        s_out0 = share_A_d_0[i] >> 16;
        s_out1 = share_A_d_1[i] >> 16;
        if(s_in != ((s_out0+s_out1) % modulus_2))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_2[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_A_share_dual_2()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_a_share(share_A_d_1, share_A_d_0, unmasked_data_d_2, len_d);
    uart_transmit_string("A MASK DUAL POW 2\n\n", 19);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        uint32_t s_in = unmasked_data_d_2[i] & 0xffff;
        uint32_t s_out0 = share_A_d_0[i] & 0xffff;
        uint32_t s_out1 = share_A_d_1[i] & 0xffff;
        if(s_in != ((s_out0+s_out1) % modulus_2))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_2[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in = unmasked_data_d_2[i] >> 16;
        s_out0 = share_A_d_0[i] >> 16;
        s_out1 = share_A_d_1[i] >> 16;
        if(s_in != ((s_out0+s_out1) % modulus_2))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_2[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 

}

void test_A2B_dual_2()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_a2b(share_B_d_1, share_B_d_0, share_A_d_1, share_A_d_0, len_d);
    uart_transmit_string("A2B DUAL POW 2\n\n", 16);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        uint32_t s_in = unmasked_data_d_2[i] & 0xffff;
        uint32_t s_out0 = share_B_d_0[i] & 0xffff;
        uint32_t s_out1 = share_B_d_1[i] & 0xffff;
        if(s_in != ((s_out0^s_out1) ))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(unmasked_data_d_2[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in = unmasked_data_d_2[i] >> 16;
        s_out0 = share_B_d_0[i] >> 16;
        s_out1 = share_B_d_1[i] >> 16;
        if(s_in != ((s_out0^s_out1) ))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(unmasked_data_d_2[i]);uart_transmit_string("\n", 1);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_A_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_d_1[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_B_d_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_B2A_single_q_1bit()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_b2a_1bit(share_A_s_1, share_A_s_0, share_B_s_1_1bit, share_B_s_0_1bit, len);
    uart_transmit_string("B2A SINGLE PRIME 1BIT\n\n", 23);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        
        uint32_t s_in_0 = (share_B_s_0_1bit[i>>5]>>(i%32))&0x1;
        uint32_t s_in_1 = (share_B_s_1_1bit[i>>5]>>(i%32))&0x1;
        if((s_in_0 ^ s_in_1) != ((share_A_s_0[i]+share_A_s_1[i]) % modulus_q))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_B_s_0_1bit[i>>5]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1_1bit[i>>5]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_B2A_dual_q_1bit()
{
    uint32_t modulus_q = 0xd01;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_q, log_modulus_q, X2X_MODULUS_PRIME, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_b2a_1bit(share_A_s_1, share_A_s_0, share_B_s_1_1bit, share_B_s_0_1bit, len_d);
    uart_transmit_string("B2A DUAL PRIME 1BIT\n\n", 21);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        
        uint32_t s_in_0 = (share_B_s_0_1bit[(i>>4)]>>((2*i)%32))&0x1;
        uint32_t s_in_1 = (share_B_s_1_1bit[(i>>4)]>>((2*i)%32))&0x1;
        uint32_t s_out0 = share_A_s_0[i] & 0xffff;
        uint32_t s_out1 = share_A_s_1[i] & 0xffff;
        if((s_in_0 ^ s_in_1) != ((s_out0 + s_out1) % modulus_q))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_B_s_0_1bit[i>>4]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1_1bit[i>>4]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            print_u32(s_in_0);uart_transmit_string("\n", 1);
            print_u32(s_in_1);uart_transmit_string("\n", 1);
            print_u32(s_out0);uart_transmit_string("\n", 1);
            print_u32(s_out1);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in_0 = (share_B_s_0_1bit[(i>>4)]>>(((2*i)%32)+1))&0x1;
        s_in_1 = (share_B_s_1_1bit[(i>>4)]>>(((2*i)%32)+1))&0x1;
        s_out0 = share_A_s_0[i] >> 16;
        s_out1 = share_A_s_1[i] >> 16;
        if((s_in_0 ^ s_in_1) != ((s_out0 + s_out1) % modulus_q))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_B_s_0_1bit[i>>4]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1_1bit[i>>4]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_B2A_single_2_1bit()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_b2a_1bit(share_A_s_1, share_A_s_0, share_B_s_1_1bit, share_B_s_0_1bit, len);
    uart_transmit_string("B2A SINGLE POW 2 1BIT\n\n", 23);
    int test = 1;
    for (unsigned int i = 0; i < len; i++)
    {   
        
        uint32_t s_in_0 = (share_B_s_0_1bit[i>>5]>>(i%32))&0x1;
        uint32_t s_in_1 = (share_B_s_1_1bit[i>>5]>>(i%32))&0x1;
        if((s_in_0 ^ s_in_1) != ((share_A_s_0[i]+share_A_s_1[i]) % modulus_2))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_B_s_0_1bit[i>>5]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1_1bit[i>>5]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

void test_B2A_dual_2_1bit()
{
    uint32_t modulus_2 = 0x1000;
    uint32_t log_modulus_q = 12;
    x2x_set_modulus(modulus_2, log_modulus_q, X2X_MODULUS_POW2, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
    x2x_b2a_1bit(share_A_s_1, share_A_s_0, share_B_s_1_1bit, share_B_s_0_1bit, len_d);
    uart_transmit_string("B2A DUAL POW 2 1BIT\n\n", 21);
    int test = 1;
    for (unsigned int i = 0; i < len_d; i++)
    {   
        
        uint32_t s_in_0 = (share_B_s_0_1bit[(i>>4)]>>((2*i)%32))&0x1;
        uint32_t s_in_1 = (share_B_s_1_1bit[(i>>4)]>>((2*i)%32))&0x1;
        uint32_t s_out0 = share_A_s_0[i] & 0xffff;
        uint32_t s_out1 = share_A_s_1[i] & 0xffff;
        if((s_in_0 ^ s_in_1) != ((s_out0 + s_out1) % modulus_2))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_B_s_0_1bit[i>>4]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1_1bit[i>>4]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }

        s_in_0 = (share_B_s_0_1bit[(i>>4)]>>(((2*i)%32)+1))&0x1;
        s_in_1 = (share_B_s_1_1bit[(i>>4)]>>(((2*i)%32)+1))&0x1;
        s_out0 = share_A_s_0[i] >> 16;
        s_out1 = share_A_s_1[i] >> 16;
        if((s_in_0 ^ s_in_1) != ((s_out0 + s_out1) % modulus_2))
        {
            uart_transmit_string("\n\n", 2);uart_transmit_string("*****\n", 6);
            print_u32(i);uart_transmit_string("\n\n", 2);
            print_u32(share_B_s_0_1bit[i>>4]);uart_transmit_string("\n", 1);
            print_u32(share_B_s_1_1bit[i>>4]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_0[i]);uart_transmit_string("\n", 1);
            print_u32(share_A_s_1[i]);uart_transmit_string("\n", 1);
            uart_transmit_string("WRONG\n\n",7);
            test = 0;
        }
    }
    if(test == 1)
        uart_transmit_string("PASS\n\n", 6); 
    else
        uart_transmit_string("FAIL\n\n", 6); 
}

int main() {
    uint32_t seed[] = {1, 1};
    x2x_seed(seed);
  
    //TEST 1 0 --> B --> A (SINGLE-2)
    test_B_share_single_2();
    test_B2A_single_2();

    //TEST 2 0 --> A --> B (SINGLE-2)
    test_A_share_single_2();
    test_A2B_single_2();
    
    //TEST 3 0 --> B --> A (DUAL-2)
    test_B_share_dual_2();
    test_B2A_dual_2();
    
    //TEST 4 0 --> A --> B (DUAL-2)
    test_A_share_dual_2();
    test_A2B_dual_2();
    
    //TEST 5 0 --> B --> A (SINGLE-q)
    test_B_share_single_q();
    test_B2A_single_q();
    
    //TEST 6 0 --> A --> B (SINGLE-q)
    test_A_share_single_q();
    test_A2B_single_q();
    
    //TEST 7 0 --> B --> A (DUAL-q)
    test_B_share_dual_q();
    test_B2A_dual_q();
    
    //TEST 8 0 --> A --> B (DUAL-q)
    test_A_share_dual_q();
    test_A2B_dual_q();

    //TEST 9 B --> A (SINGLE-q 1bit)
    test_B2A_single_q_1bit();

    //TEST 10 B --> A (DUAL-q 1bit)
    test_B2A_dual_q_1bit();
    
    test_B2A_single_2_1bit();
    test_B2A_dual_2_1bit();

    
    return 0;
}
