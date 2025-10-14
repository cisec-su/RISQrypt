#ifndef SDK_BENCHMARK_H
#define SDK_BENCHMARK_H

#include "timer.h"

#define BENCH_CONST  5

#define BENCH_START(g)       g -= timer_read();

#define BENCH_END(g)         g += timer_read() + BENCH_CONST;


#define BENCH_START_LOCK(g, l)       g -= timer_read(); \
                                     while(l); l = 1;


#define BENCH_END_LOCK(g, l)         g += timer_read() + BENCH_CONST; \
                                     l = 0;


#endif