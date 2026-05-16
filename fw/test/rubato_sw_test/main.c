#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "util.h"
#include "timer.h"
#include "unity.h"
#include "benchmark.h"
#include "rubato_soft.h"
#include "masked_rubato_soft.h"
#include <stddef.h>
#include <stdint.h>
#include "fips202.h"

////////////////////////////////////////////////////////////////
/**
    secret_key = [1]*256
    plaintext  = [0]*RUBATO_OUTPUTSIZE
    nonce = 0x0123456789ABCDEF
*/
#if defined(PARAM_80_S) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x2EEAD49, 0x38AD4DF, 0x174FD0E, 0x1035773,
        0x1374125, 0x2BF8C50, 0x34F1EF8, 0x242ABB4,
        0x25CB86D, 0x09678F5, 0x3031EA8, 0x2648BB5,
    };
#elif defined(PARAM_80_S) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x3272AAF, 0x1F96DF9, 0x0E3A7E2, 0x207701F,
        0x0A57EC9, 0x19A4BC4, 0x330798C, 0x1027104,
        0x1F57641, 0x121B5AE, 0x35C374F, 0x1DB86C0,
    };

#elif defined(PARAM_80_M) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x1CB0062, 0x039F975, 0x0E982AE, 0x1D8194F,
        0x0CE0A00, 0x168C14B, 0x061B3BB, 0x0227B26,
        0x15D9FED, 0x0366713, 0x0A75462, 0x15F6FB1,
        0x1003F8F, 0x10F32B1, 0x1057891, 0x0F9CCD2,
        0x1EBF4F0, 0x155469E, 0x0E30672, 0x15A20BD,
        0x129DEFB, 0x13FBD1A, 0x0E46A32, 0x1668A21,
        0x15F0031, 0x10CB036, 0x06064DA, 0x0179ECD,
        0x18D6F5D, 0x12296F6, 0x0405B87, 0x1EBD4C0,
    };
#elif defined(PARAM_80_M) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x05D9302, 0x090F7F1, 0x0747596, 0x0462A31,
        0x0C92575, 0x00AB447, 0x04AD7A8, 0x18F4739,
        0x03EF5DF, 0x074FA00, 0x1A6D129, 0x0D2C02C,
        0x0FA7397, 0x1D59164, 0x1E42309, 0x0B1E0DA,
        0x16B68B4, 0x1F30E8D, 0x109DD1C, 0x1A84066,
        0x16533CD, 0x09ED237, 0x0AEB642, 0x1BE067A,
        0x0ECA7F6, 0x174BFAD, 0x06473B0, 0x14932CB,
        0x1582742, 0x1668EC3, 0x0567D55, 0x10313C4,
    };

#elif defined(PARAM_80_L) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x12B6FFA, 0x03B5CA7, 0x12A3EC9, 0x095878B,
        0x002C044, 0x0E9EDDA, 0x1CDCBDB, 0x08CDC02,
        0x09EED2F, 0x045E16E, 0x0F1DBDC, 0x1F4D3D0,
        0x123F5B3, 0x103FCC2, 0x1069737, 0x0C3C8F7,
        0x18F0737, 0x149CE3E, 0x14B0355, 0x133937D,
        0x041772B, 0x09DFF06, 0x1AD0B5E, 0x043A7BB,
        0x19767B6, 0x1826591, 0x08F8DE9, 0x1888AD2,
        0x1D5DD6D, 0x1066A85, 0x0BE06E1, 0x12C805B,
        0x0A3B17F, 0x1A3FDAB, 0x000BBFA, 0x1E18F6D,
        0x05AE938, 0x19AF4AF, 0x19C3C0F, 0x15811A2,
        0x1D40DBE, 0x040E0F6, 0x003C8A0, 0x052F7BA,
        0x1BD81D2, 0x177337F, 0x0B8894D, 0x1CA48FC,
        0x034B1FE, 0x1C9D4A2, 0x14CE2E7, 0x18A244D,
        0x03C0970, 0x1776763, 0x140A479, 0x1232283,
        0x1620633, 0x080E66A, 0x06E6485, 0x16D9E9E,
    };
#elif defined(PARAM_80_L) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x0467D33, 0x0BF04AA, 0x0D5CA49, 0x13F3AB2,
        0x011637B, 0x1E0CA02, 0x14C480A, 0x0BC8390,
        0x11FCC17, 0x10028DB, 0x0D1C770, 0x0A88361,
        0x03EB03A, 0x1816A1C, 0x0566FCD, 0x1F202DA,
        0x12A61C8, 0x064A2D6, 0x0DDED15, 0x00650D9,
        0x1BB3DCE, 0x17B9D72, 0x01D1E4E, 0x1F42118,
        0x00056E64, 0x104BC88, 0x1240035, 0x01F8C84,
        0x19AFCC3, 0x04BD6DD, 0x18F5C84, 0x042D166,
        0x09EF734, 0x1C73949, 0x0B226A3, 0x1DF5D51,
        0x0DBB9F4, 0x1297810, 0x1594334, 0x0C99AB2,
        0x13669CF, 0x173E124, 0x01DAD49, 0x1386F74,
        0x1383D70, 0x05B7851, 0x165E24E, 0x13CDB68,
        0x13C3A5E, 0x1B5BD2A, 0x17DE8F6, 0x11DD095,
        0x1B5EBFF, 0x07206C0, 0x1F70DB5, 0x10DE1AE,
        0x021F9B3, 0x1172D97, 0x0D040D3, 0x035136B,
    };

#elif defined(PARAM_128_S) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x046D60D, 0x19627E3, 0x1461439, 0x30E828A,
        0x08A8E5F, 0x3C09C3A, 0x07664FC, 0x1FF5C7F,
        0x3217E3D, 0x28AF373, 0x124C3A5, 0x058979D,
    };
#elif defined(PARAM_128_S) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x11538EC, 0x1FE6F22, 0x189FE6B, 0x18A10A9,
        0x2F718FC, 0x33DF5DE, 0x234254A, 0x3C5DB5E,
        0x1AD31B1, 0x11B598A, 0x20602B1, 0x3CDACB6,
    };

#elif defined(PARAM_128_M) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x190FB0E, 0x18BDED0, 0x170CBDF, 0x02AAB29,
        0x17DF55C, 0x1A39694, 0x1C5422B, 0x1F10A2D,
        0x1327275, 0x1761B8E, 0x13FFC1D, 0x144F93A,
        0x060D556, 0x1AC3A60, 0x1BFCAE7, 0x1DACF1A,
        0x051035C, 0x094E492, 0x08EC813, 0x18340E1,
        0x044F3ED, 0x1306B76, 0x0215536, 0x0DC88E6,
        0x0A9B5D7, 0x0EF3CD1, 0x1FAEDFF, 0x1C195D3,
        0x14865D3, 0x1DB1C01, 0x0085FBA, 0x093F436,
    };
#elif defined(PARAM_128_M) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x1ABA327, 0x13DC626, 0x03912F6, 0x08C1AC6,
        0x14F5DDC, 0x075953D, 0x147935B, 0x0D8A88B,
        0x0BBBEAE, 0x09104E7, 0x10341D1, 0x13C864B,
        0x0419D67, 0x1E10CB3, 0x1B0801D, 0x0F5C0E9,
        0x1CCFEB5, 0x1BDBBFF, 0x1245D47, 0x00078FB,
        0x05DC203, 0x02A99A5, 0x12E8F71, 0x03F3B84,
        0x0691A57, 0x16D2825, 0x1342F1D, 0x1E9AF64,
        0x133489C, 0x0F1E278, 0x1568B74, 0x00FA172,
    };

#elif defined(PARAM_128_L) && defined(XOF_SHAKE128)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x12B6FFA, 0x03B5CA7, 0x12A3EC9, 0x095878B,
        0x002C044, 0x0E9EDDA, 0x1CDCBDB, 0x08CDC02,
        0x09EED2F, 0x045E16E, 0x0F1DBDC, 0x1F4D3D0,
        0x123F5B3, 0x103FCC2, 0x1069737, 0x0C3C8F7,
        0x18F0737, 0x149CE3E, 0x14B0355, 0x133937D,
        0x041772B, 0x09DFF06, 0x1AD0B5E, 0x043A7BB,
        0x19767B6, 0x1826591, 0x08F8DE9, 0x1888AD2,
        0x1D5DD6D, 0x1066A85, 0x0BE06E1, 0x12C805B,
        0x0A3B17F, 0x1A3FDAB, 0x000BBFA, 0x1E18F6D,
        0x05AE938, 0x19AF4AF, 0x19C3C0F, 0x15811A2,
        0x1D40DBE, 0x040E0F6, 0x003C8A0, 0x052F7BA,
        0x1BD81D2, 0x177337F, 0x0B8894D, 0x1CA48FC,
        0x034B1FE, 0x1C9D4A2, 0x14CE2E7, 0x18A244D,
        0x03C0970, 0x1776763, 0x140A479, 0x1232283,
        0x1620633, 0x080E66A, 0x06E6485, 0x16D9E9E,
    };
#elif defined(PARAM_128_L) && defined(XOF_SHAKE256)
    static const uint32_t expected[RUBATO_OUTPUTSIZE] = {
        0x0467D33, 0x0BF04AA, 0x0D5CA49, 0x13F3AB2,
        0x011637B, 0x1E0CA02, 0x14C480A, 0x0BC8390,
        0x11FCC17, 0x10028DB, 0x0D1C770, 0x0A88361,
        0x03EB03A, 0x1816A1C, 0x0566FCD, 0x1F202DA,
        0x12A61C8, 0x064A2D6, 0x0DDED15, 0x00650D9,
        0x1BB3DCE, 0x17B9D72, 0x01D1E4E, 0x1F42118,
        0x00056E64, 0x104BC88, 0x1240035, 0x01F8C84,
        0x19AFCC3, 0x04BD6DD, 0x18F5C84, 0x042D166,
        0x09EF734, 0x1C73949, 0x0B226A3, 0x1DF5D51,
        0x0DBB9F4, 0x1297810, 0x1594334, 0x0C99AB2,
        0x13669CF, 0x173E124, 0x01DAD49, 0x1386F74,
        0x1383D70, 0x05B7851, 0x165E24E, 0x13CDB68,
        0x13C3A5E, 0x1B5BD2A, 0x17DE8F6, 0x11DD095,
        0x1B5EBFF, 0x07206C0, 0x1F70DB5, 0x10DE1AE,
        0x021F9B3, 0x1172D97, 0x0D040D3, 0x035136B,
    };

#else
#error "No expected SHAKE test vector for selected PARAM/XOF."
#endif

#if (RUBATO_BLOCKSIZE == 16)
#  define RUBATO_KEY_INIT {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
#elif (RUBATO_BLOCKSIZE == 36)
#  define RUBATO_KEY_INIT {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
#elif (RUBATO_BLOCKSIZE == 64)
#  define RUBATO_KEY_INIT {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
#else
#  error "Unsupported RUBATO_BLOCKSIZE for RUBATO_KEY_INIT"
#endif

const uint32_t RUBATO_KEY[RUBATO_BLOCKSIZE] = RUBATO_KEY_INIT;

static const int32_t RUBATO_PLAINTEXT[RUBATO_OUTPUTSIZE] = {0};

const uint64_t RUBATO_NONCE = 0x0123456789ABCDEFULL;

const uint64_t RUBATO_BLOCK_CTR = 0x0;

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

void rubato_test() {

    poly ciphertext;
    poly plaintext;
    poly key;
    size_t i;
    poly coeffs[RUBATO_R + 1];

    print_string("\n --- rubato_soft_encrypt test --- \n");
    for (size_t i = 0; i < RUBATO_OUTPUTSIZE; i++)
    {
        plaintext.coeffs[i] = 0;
    }
    
    for (size_t i = 0; i < N; i++)
    {
        key.coeffs[i] = RUBATO_KEY[i];
    }
    
    BENCH_INIT()

    BENCH_START()
    rubato_soft_encrypt(&ciphertext, &plaintext, &key, RUBATO_NONCE, RUBATO_BLOCK_CTR);
    BENCH_END(RUBATO_SOFT_ENCRYPT)
    // TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, (uint32_t *)ciphertext.coeffs, RUBATO_OUTPUTSIZE);

    // BENCH_START()
    // rubato_soft_sampling(coeffs, RUBATO_NONCE, RUBATO_BLOCK_CTR);
    // BENCH_END(RUBATO_SOFT_SAMPLING)

    // BENCH_START()
    // rubato_soft_linear_layer(&ciphertext, &ciphertext);
    // BENCH_END(RUBATO_SOFT_LINEAR_LAYER)
    
    // print_u32_arr(expected, RUBATO_OUTPUTSIZE);
    // print_string("\n*******\n");
    // print_u32_arr((uint32_t *)ciphertext, RUBATO_OUTPUTSIZE);

}
////////////////////////////////////////////////////////////////
void rubato_soft_sampling_test() {

    poly coeffs[RUBATO_R + 1];
    uint64_t nonce = 0x0123456789ABCDEFULL;
    uint64_t block_ctr = 0x0;

    print_string("\n --- rubato_soft_sampling test --- \n");

    BENCH_INIT()

    /* generate one software keystream block */
    BENCH_START()
    rubato_soft_sampling(coeffs, nonce, block_ctr);
    BENCH_END(RUBATO_SOFT_SAMPLING)

}
////////////////////////////////////////////////////////////////
void masked_rubato_test() {

    poly ciphertext;
    poly plaintext;
    poly key;
    size_t i;

    print_string("\n --- rubato_soft_encrypt test --- \n");
    for (size_t i = 0; i < RUBATO_OUTPUTSIZE; i++)
    {
        plaintext.coeffs[i] = 0;
    }
    
    for (size_t i = 0; i < N; i++)
    {
        key.coeffs[i] = RUBATO_KEY[i];
    }
    
    BENCH_INIT()

    BENCH_START()
    masked_rubato_soft_encrypt(&ciphertext, &plaintext, &key, RUBATO_NONCE, RUBATO_BLOCK_CTR);
    BENCH_END(MASKED_RUBATO_SOFT_ENCRYPT)
    // TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, (uint32_t *)ciphertext.coeffs, RUBATO_OUTPUTSIZE);

}
////////////////////////////////////////////////////////////////

int main() {
    UnityBegin("main.c");
    print_string("\n --- Rubato Test Start --- \n");
    // RUN_TEST(rubato_soft_sampling_test);
    RUN_TEST(rubato_test);
    RUN_TEST(masked_rubato_test);
    return(UnityEnd());
}