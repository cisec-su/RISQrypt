#define PASTA_PLAIN_SIZE 128
#define PASTA_KEY_SIZE (2 * PASTA_PLAIN_SIZE)   
#define N PASTA_PLAIN_SIZE

#define PASTA_R 3
#define Q 65537
#define MODULUS_BIT_MASK (1<<17)-1 // 0x1FFFF

#define PASTA_NAMESPACE(s) pasta_##s

#define MASKING_N 2
#define LOG_MASKING_N 1
