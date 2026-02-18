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

