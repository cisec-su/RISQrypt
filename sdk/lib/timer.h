#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

void timer_start();

void timer_reset();

uint64_t timer_read();

#endif