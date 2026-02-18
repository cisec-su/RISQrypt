#include "risqrypt.h"
#include "x2x_prng_regs.h"


void x2x_prng_off(void)
{
    X2X_REGS->ctrl |= X2X_CTRL_PRNG_OFF_V;
}