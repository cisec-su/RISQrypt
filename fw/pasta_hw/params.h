
#include "config.h"

#define NONCE_CONST 123456789
#define SHAKE_BUFFER_SIZE 131072
#define PASTA_PLAIN_SIZE 128
#define PASTA_KEY_SIZE (2 * PASTA_PLAIN_SIZE)   
#define N PASTA_PLAIN_SIZE

#if PASTA_MODE == 3
#define PASTA_R 3
#define ROOT_OF_UNITY 1753  //?
#define Q 65537
#define MODULUS_BIT_MASK (1<<17)-1 // 0x1FFFF
#endif


