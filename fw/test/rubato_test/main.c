#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "util.h"
#include "poly.h"
#include "timer.h"
#include "unity.h"
#include "benchmark.h"
#include "rubato.h"
#include "masked_rubato.h"
#include "ntt_lite.h"

////////////////////////////////////////////////////////////////
/**
    secret_key = [1]*256
    plaintext  = [1]*128
    nonce = 0x0123456789ABCDEF
*/


#if defined(PARAM_80_S) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x1EA6070, 0x00E9D2C, 0x3285C49, 0x20A933F,
        0x325D29C, 0x1878A86, 0x20FB87A, 0x177AD9C,
        0x02D89CE, 0x07884DA, 0x1490D58, 0x119E679,
    };
#elif defined(PARAM_80_S) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x1EA6070, 0x00E9D2C, 0x3285C49, 0x20A933F,
        0x325D29C, 0x1878A86, 0x20FB87A, 0x177AD9C,
        0x02D89CE, 0x07884DA, 0x1490D58, 0x119E679,
    };

#elif defined(PARAM_80_L) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x15FEF9A, 0x068B52D, 0x1525C03, 0x0C3457C,
        0x1A4629B, 0x1A27BE0, 0x1C418CD, 0x08BA404,
        0x040573E, 0x1CE3055, 0x0E1EB75, 0x04C84B7,
        0x004A8AC, 0x13844FD, 0x1C113E6, 0x18004D0,
        0x1F5DAAE, 0x1DD0CCC, 0x0215479, 0x11D8271,
        0x035676D, 0x0C25F17, 0x0CC6231, 0x0D832A9,
        0x0504B89, 0x0C48275, 0x0A36133, 0x1C4B416,
        0x1E290EF, 0x1F42AB7, 0x0350D8E, 0x1E61255,
        0x16B5B46, 0x13E7C7C, 0x066AC93, 0x1698ABF,
        0x096F7A0, 0x0D6919F, 0x0EF8B31, 0x13BE077,
        0x12D5A90, 0x0FF3D4A, 0x165623D, 0x0CE7EE2,
        0x1D13DB7, 0x0D8D000, 0x1DAE7F8, 0x0D044D4,
        0x196D09C, 0x0E53565, 0x1033AE6, 0x072D891,
        0x0F737E6, 0x0204DC0, 0x042D916, 0x0E4B5B1,
        0x07593D6, 0x1D730FC, 0x195C461, 0x01EE51B,
    };
#elif defined(PARAM_80_L) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x15FEF9A, 0x068B52D, 0x1525C03, 0x0C3457C,
        0x1A4629B, 0x1A27BE0, 0x1C418CD, 0x08BA404,
        0x040573E, 0x1CE3055, 0x0E1EB75, 0x04C84B7,
        0x004A8AC, 0x13844FD, 0x1C113E6, 0x18004D0,
        0x1F5DAAE, 0x1DD0CCC, 0x0215479, 0x11D8271,
        0x035676D, 0x0C25F17, 0x0CC6231, 0x0D832A9,
        0x0504B89, 0x0C48275, 0x0A36133, 0x1C4B416,
        0x1E290EF, 0x1F42AB7, 0x0350D8E, 0x1E61255,
        0x16B5B46, 0x13E7C7C, 0x066AC93, 0x1698ABF,
        0x096F7A0, 0x0D6919F, 0x0EF8B31, 0x13BE077,
        0x12D5A90, 0x0FF3D4A, 0x165623D, 0x0CE7EE2,
        0x1D13DB7, 0x0D8D000, 0x1DAE7F8, 0x0D044D4,
        0x196D09C, 0x0E53565, 0x1033AE6, 0x072D891,
        0x0F737E6, 0x0204DC0, 0x042D916, 0x0E4B5B1,
        0x07593D6, 0x1D730FC, 0x195C461, 0x01EE51B,
    };

#elif defined(PARAM_128_S) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x1BB8109, 0x0CE9EF8, 0x3B76435, 0x063161E,
        0x203B422, 0x1EA24FC, 0x22CFB65, 0x0CD121C,
        0x0F2FF53, 0x145C0F8, 0x1917E78, 0x1602D23,
    };
#elif defined(PARAM_128_S) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x1BB8109, 0x0CE9EF8, 0x3B76435, 0x063161E,
        0x203B422, 0x1EA24FC, 0x22CFB65, 0x0CD121C,
        0x0F2FF53, 0x145C0F8, 0x1917E78, 0x1602D23,
    };

#elif defined(PARAM_128_L) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x15FEF9A, 0x068B52D, 0x1525C03, 0x0C3457C,
        0x1A4629B, 0x1A27BE0, 0x1C418CD, 0x08BA404,
        0x040573E, 0x1CE3055, 0x0E1EB75, 0x04C84B7,
        0x004A8AC, 0x13844FD, 0x1C113E6, 0x18004D0,
        0x1F5DAAE, 0x1DD0CCC, 0x0215479, 0x11D8271,
        0x035676D, 0x0C25F17, 0x0CC6231, 0x0D832A9,
        0x0504B89, 0x0C48275, 0x0A36133, 0x1C4B416,
        0x1E290EF, 0x1F42AB7, 0x0350D8E, 0x1E61255,
        0x16B5B46, 0x13E7C7C, 0x066AC93, 0x1698ABF,
        0x096F7A0, 0x0D6919F, 0x0EF8B31, 0x13BE077,
        0x12D5A90, 0x0FF3D4A, 0x165623D, 0x0CE7EE2,
        0x1D13DB7, 0x0D8D000, 0x1DAE7F8, 0x0D044D4,
        0x196D09C, 0x0E53565, 0x1033AE6, 0x072D891,
        0x0F737E6, 0x0204DC0, 0x042D916, 0x0E4B5B1,
        0x07593D6, 0x1D730FC, 0x195C461, 0x01EE51B,
    };
#elif defined(PARAM_128_L) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x15FEF9A, 0x068B52D, 0x1525C03, 0x0C3457C,
        0x1A4629B, 0x1A27BE0, 0x1C418CD, 0x08BA404,
        0x040573E, 0x1CE3055, 0x0E1EB75, 0x04C84B7,
        0x004A8AC, 0x13844FD, 0x1C113E6, 0x18004D0,
        0x1F5DAAE, 0x1DD0CCC, 0x0215479, 0x11D8271,
        0x035676D, 0x0C25F17, 0x0CC6231, 0x0D832A9,
        0x0504B89, 0x0C48275, 0x0A36133, 0x1C4B416,
        0x1E290EF, 0x1F42AB7, 0x0350D8E, 0x1E61255,
        0x16B5B46, 0x13E7C7C, 0x066AC93, 0x1698ABF,
        0x096F7A0, 0x0D6919F, 0x0EF8B31, 0x13BE077,
        0x12D5A90, 0x0FF3D4A, 0x165623D, 0x0CE7EE2,
        0x1D13DB7, 0x0D8D000, 0x1DAE7F8, 0x0D044D4,
        0x196D09C, 0x0E53565, 0x1033AE6, 0x072D891,
        0x0F737E6, 0x0204DC0, 0x042D916, 0x0E4B5B1,
        0x07593D6, 0x1D730FC, 0x195C461, 0x01EE51B,
    };

#else
#error "No expected SHAKE test vector for selected PARAM/XOF."
#endif

#if (RUBATO_BLOCKSIZE == 16)
#  define RUBATO_KEY_INIT {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
#elif (RUBATO_BLOCKSIZE == 64)
#  define RUBATO_KEY_INIT {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
#else
#  error "Unsupported RUBATO_BLOCKSIZE for RUBATO_KEY_INIT"
#endif

const uint32_t RUBATO_KEY[RUBATO_BLOCKSIZE] = RUBATO_KEY_INIT;

const uint64_t RUBATO_NONCE = 0x0123456789ABCDEFULL;

const uint64_t RUBATO_BLOCK_CTR = 0x0;

/* Keystream buffer placed in .bss instead of stack — short logn=4 HW ops
   appear to race with stack-resident DMA targets when the caller reads
   immediately after rubato_encrypt returns. */

////////////////////////////////////////////////////////////////
/**
 * @brief Unity setup hook.
 */
void setUp(void)
{
}

/**
 * @brief Unity teardown hook.
 */
void tearDown(void)
{
}
////////////////////////////////////////////////////////////////

/**
 * @brief Tests and benchmarks software RUBATO keystream generation.
 */
void rubato_test() {
    poly plaintext;
    poly ciphertext;
    poly key;
    poly rnd;
    size_t i;
    uint8_t poly_ctr = 0;

    for (i = 0; i < N; i++) {
        plaintext.coeffs[i] = 0;
    }
    for (i = 0; i < N; i++) {
        key.coeffs[i] = RUBATO_KEY[i];
    }

    BENCH_INIT()
    BENCH_START()
    rubato_encrypt(&ciphertext, &plaintext, &key, RUBATO_NONCE, RUBATO_BLOCK_CTR);
    BENCH_END(RUBATO_ENCRYPT)
    TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, ciphertext.coeffs, RUBATO_OUTPUTSIZE);

    
    BENCH_START()
    poly_uniform(&rnd, RUBATO_NONCE, RUBATO_BLOCK_CTR, poly_ctr);
    BENCH_END(POLY_UNIFORM)

    BENCH_START()
    rubato_linear_layer(&rnd, &rnd);
    BENCH_END(RUBATO_LINEAR_LAYER)

    // print_string("\nExpected:\n");
    // print_u32_arr(expected, RUBATO_OUTPUTSIZE);
    // print_string("\nGot:\n");
    // print_u32_arr(ciphertext.coeffs, RUBATO_OUTPUTSIZE);    
    // TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, ciphertext.coeffs, RUBATO_OUTPUTSIZE);
}

/**
 * @brief Tests and benchmarks software RUBATO keystream generation.
 */
void masked_rubato_test() {
    poly plaintext;
    poly ciphertext;
    poly key;
    poly rnd;
    size_t i;
    uint8_t poly_ctr = 0;

    for (i = 0; i < N; i++) {
        plaintext.coeffs[i] = 0;
    }
    for (i = 0; i < N; i++) {
        key.coeffs[i] = RUBATO_KEY[i];
    }

    BENCH_INIT()
    BENCH_START()
    masked_rubato_encrypt(&ciphertext, &plaintext, &key, RUBATO_NONCE, RUBATO_BLOCK_CTR);
    BENCH_END(MASKED_RUBATO_ENCRYPT)
    TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, ciphertext.coeffs, RUBATO_OUTPUTSIZE);

}


int main() {

    UnityBegin("main.c");
    print_string("\n --- Rubato Test Start --- \n");
    RUN_TEST(rubato_test);
    RUN_TEST(masked_rubato_test);
    return(UnityEnd());
}