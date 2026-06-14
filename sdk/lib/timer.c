#include "timer.h"
#include "risqrypt.h"

void timer_start() {
    timer_reset();
    TIMER_REGS->ctrl = TIMER_START_V;
}

void timer_reset() {
    TIMER_REGS->ctrl = TIMER_RESET_V;
}

uint32_t timer_read() {
    return TIMER_REGS->data;
}

uint64_t timer_read() {
    uint32_t hi, lo, hi2;
    do {
        hi  = TIMER_REGS->data_hi;
        lo  = TIMER_REGS->data;
        hi2 = TIMER_REGS->data_hi;
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | lo;
}