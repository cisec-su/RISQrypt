#ifndef SDK_BENCHMARK_H
#define SDK_BENCHMARK_H

#include "timer.h"

#ifndef BENCH_CONST
#define BENCH_CONST  5
#endif

#ifdef SDK_BENCHMARK_ENABLED
#define BENCH_START(g)       g -= timer_read();

#define BENCH_END(g)         g += timer_read() + BENCH_CONST;
#else
#define BENCH_START(g)
#define BENCH_END(g)
#endif


#define BENCH_START_LOCK(g, l)       g -= timer_read(); \
                                     while(l); l = 1;


#define BENCH_END_LOCK(g, l)         g += timer_read() + BENCH_CONST; \
                                     l = 0;


#endif