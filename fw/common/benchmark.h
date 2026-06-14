//#ifndef BENCHMARK_H
//#define BENCHMARK_H

//FUNCTION LABEL HELPER FUNCTIONS
#define XSTR(A) #A
#define STR(A) XSTR(A)

#define TEST_NAME_EXPANDED(name) TEST_NAME(name)
#define TEST_NAME(name) test_##name 

#define ANSI_RESET   "\x1b[0m"
#define ANSI_GREEN   "\x1b[32m"

//TIMER MACROS FOR BENCHMARKING
#define BENCH_INIT()        uint64_t time;
#define BENCH_START()       timer_start();

#define BENCH_END_SHIFT(name, d)    time = timer_read(); \
                                    print_string(STR(name) ":\t"); \
                                    print_u64_int(time >> d); \
                                    print_string(" cycles"); \
                                    print_string("\n");

#define BENCH_END(name)     BENCH_END_SHIFT(name, 0);

#define BENCH_COL_CYCLES 27

#define BENCH_PAD(label) \
    { \
        const unsigned _len = (unsigned)(sizeof(STR(label)) - 1); \
        for (unsigned _i = _len; _i < BENCH_COL_CYCLES; _i++) print_string(" "); \
    }

#define BENCH_END_SHIFT_COL_T(name, d, time)    print_string(STR(name)); \
                                        BENCH_PAD(name); \
                                        print_string(ANSI_GREEN); \
                                        print_u64_int(time >> (d)); \
                                        print_string(ANSI_RESET); \
                                        print_string("\n");

#define BENCH_END_SHIFT_COL(name, d)    time = timer_read(); \
                                        BENCH_END_SHIFT_COL_T(name, d, time)

#define BENCH_END_COL(name)     BENCH_END_SHIFT_COL(name, 0)

#define BENCH_PAD_STRLEN(_len) \
    for (unsigned _i = (unsigned)(_len); _i < (unsigned)BENCH_COL_CYCLES; _i++) print_string(" ");

#define BENCH_HEADER() \
    print_string("Function"); \
    BENCH_PAD_STRLEN(sizeof("Function") - 1); \
    print_string("Cycles\n");

#define BENCH_LINE() \
    print_string("-----------------------------------\n");

#define BENCH_TITLE(alg_str) \
    print_string("\n"); \
    print_string(alg_str); \
    print_string(" TIME PERF BENCHMARKING\n");

#define BENCH_DONE() \
    print_string("Finished\n");

//#endif //BENCHMARK_H