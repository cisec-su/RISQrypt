
//FUNCTION LABEL HELPER FUNCTIONS
#define XSTR(A) #A
#define STR(A) XSTR(A)

#define TEST_NAME_EXPANDED(name) TEST_NAME(name)
#define TEST_NAME(name) test_##name 

//TIMER MACROS FOR BENCHMARKING
#define BENCH_INIT()        unsigned int time;
#define BENCH_START()       timer_start();

#define BENCH_END_SHIFT(name, d)    time = timer_read(); \
                                    print_string(STR(name) ":\t"); \
                                    print_u32_int(time >> d); \
                                    print_string(" cycles"); \
                                    print_string("\n");


#define BENCH_END(name)     BENCH_END_SHIFT(name, 0)                         