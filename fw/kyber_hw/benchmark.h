
//FUNCTION LABEL HELPER FUNCTIONS
#define XSTR(A) #A
#define STR(A) XSTR(A)

#define TEST_NAME_EXPANDED(name) TEST_NAME(name)
#define TEST_NAME(name) test_##name 

/*----------------------------------------------------------------------------------------------------- */
//FUNCTION LABELS
/*----------------------------------------------------------------------------------------------------- */
#define MASKED_INDCPA_ENC_CMP   masked_indcpa_enc_cmp   //CHECK github actions and function labeling
/*----------------------------------------------------------------------------------------------------- */
#define MASKED_INDCPA_DEC       masked_indcpa_dec
/*----------------------------------------------------------------------------------------------------- */
#define MASKED_CBD              masked_cbd
/*----------------------------------------------------------------------------------------------------- */
#define MASKED_POLY_COMPRESS    masked_poly_compress    // belongs to masked_poly_compress
#define MASKED_POLY_COMPRESS_DU masked_poly_compress_du // belongs to masked_poly_compress
/*----------------------------------------------------------------------------------------------------- */
#define MASKED_POLY_MSG         masked_poly_msg 
/*----------------------------------------------------------------------------------------------------- */
#define MASKED_POLY_MSG         masked_poly_msg         // INDCCA consists of below
#define MASKED_POLY_SUB_TOMSG   masked_poly_sub_tomsg   
#define MASKED_POLY_SUB_FROMSG  masked_poly_sub_frommsg 
/*----------------------------------------------------------------------------------------------------- */
#define INDCCA                  indcca              // INDCCA consists of below
#define CRYPTO_KEM_KEYPAIR      crypto_kem_keypair  
#define CRYPTO_KEM_ENC          crypto_kem_enc      
#define CRYPTO_KEM_DEC          crypto_kem_dec  
/*----------------------------------------------------------------------------------------------------- */
#define INDCPA_KEYPAIR          indcpa_keypair      
/*----------------------------------------------------------------------------------------------------- */
#define INDCPA_ENC              indcpa_enc      
/*----------------------------------------------------------------------------------------------------- */
#define INDCPA_DEC              indcpa_dec      
/*----------------------------------------------------------------------------------------------------- */


//TIMER MACROS FOR BENCHMARKING
#define BENCH_INIT()        unsigned int time;
#define BENCH_START()       timer_start();

#define BENCH_END(name)     print_string(STR(name) ":\t"); \
                            time = timer_read(); \
                            print_u32_int(time); \
                            print_string(" cycles"); \
                            print_string("\n");

           