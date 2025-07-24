
//FUNCTION LABEL HELPER FUNCTIONS
#define XSTR(A) #A
#define STR(A) XSTR(A)

#define TEST_NAME_EXPANDED(name) TEST_NAME(name)
#define TEST_NAME(name) test_##name 

/*----------------------------------------------------------------------------------------------------- */
//FUNCTION LABELS
/*----------------------------------------------------------------------------------------------------- */
#define test_masked_poly_compress_du    test_masked_poly_compress_du // belongs to masked_poly_compress
/*----------------------------------------------------------------------------------------------------- */
#define test_masked_poly_sub_tomsg      test_masked_poly_sub_tomsg   
#define test_masked_poly_sub_frommsg    test_masked_poly_sub_frommsg 
/*----------------------------------------------------------------------------------------------------- */
#define test_crypto_kem_keypair         test_crypto_kem_keypair  
#define test_crypto_kem_enc             test_crypto_kem_enc      
#define test_crypto_kem_dec             test_crypto_kem_dec  
/*----------------------------------------------------------------------------------------------------- */


//TIMER MACROS FOR BENCHMARKING
#define BENCH_INIT()        unsigned int time;
#define BENCH_START()       timer_start();

#define BENCH_END(name)     print_string(STR(name) ":\t"); \
                            time = timer_read(); \
                            print_u32_int(time); \
                            print_string(" cycles"); \
                            print_string("\n");

           