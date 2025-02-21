#include "x2x.h"
#include "hornet.h"



int x2x_set_modulus(uint32_t *modulus, unsigned int len, unsigned int modulus_type)
{

    if (len > 1) {
        return -1;
    }

    X2X_REGS->ctrl |= X2X_CTRL_RESET_V;

    while ((X2X_REGS->ctrl & X2X_CTRL_DONE_V) == 0);


    if (modulus_type == X2X_MODULUS_POW2) {
        X2X_REGS->ctrl = (X2X_REGS->ctrl & ~X2X_CTRL_DATA_TYPE_M) | X2X_CTRL_DATA_TYPE_POW2;
    } else if (modulus_type == X2X_MODULUS_PRIME) {
        X2X_REGS->ctrl = (X2X_REGS->ctrl & ~X2X_CTRL_DATA_TYPE_M) | X2X_CTRL_DATA_TYPE_PRIME;
    } else {
        return -1;
    }


    X2X_REGS->modulus[0] = modulus[0];

    if (len == 2) {
        X2X_REGS->modulus[1] = modulus[1];
    }

    return 0;
}


static int x2x_core(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src0, unsigned int len, uint32_t conv_mode)
{

    X2X_REGS->ctrl |= X2X_CTRL_RESET_V;

    while ((X2X_REGS->ctrl & X2X_CTRL_DONE_V) == 0);

    X2X_REGS->data_len = len;
    X2X_REGS->din_addr[0] = (uint32_t) src0;
    X2X_REGS->din_addr[1] = (uint32_t) src_1;
    X2X_REGS->dout_addr[0] = (uint32_t) dst_0;
    X2X_REGS->dout_addr[1] = (uint32_t) dst_1;

    X2X_REGS->ctrl = (X2X_REGS->ctrl & ~X2X_CTRL_CONV_MODE_M) | conv_mode;
    X2X_REGS->ctrl |= X2X_CTRL_START_V;

    while ((X2X_REGS->ctrl & X2X_CTRL_DONE_V) == 0);

    return 0;
}


int x2x_a2b(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src0, unsigned int len)
{
    return x2x_core(dst_1, dst_0, src_1, src0, len, X2X_CTRL_CONV_MODE_A2B);
}


int x2x_b2a(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src0, unsigned int len)
{
    return x2x_core(dst_1, dst_0, src_1, src0, len, X2X_CTRL_CONV_MODE_B2A);
}