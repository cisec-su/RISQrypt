#include "x2x.h"
#include "hornet.h"



int x2x_set_modulus(uint32_t *modulus, uint32_t modulus_type, uint32_t dual_mode)
{
    uint32_t dual_flag;

    if (dual_mode == X2X_DUAL_MODE_EN) {
        if (modulus_type == X2X_CTRL_DATA_TYPE_PRIME) {
            return -1;
        }
        else {
            dual_flag = X2X_CTRL_DUAL_MODE_EN_S;
        }
    } else if (dual_mode == X2X_DUAL_MODE_DIS) {
        dual_flag = X2X_CTRL_DUAL_MODE_EN_S;
    } else {
        return -1;
    }

    X2X_REGS->ctrl |= X2X_CTRL_RESET_V;

    while ((X2X_REGS->ctrl & X2X_CTRL_DONE_V) == 0);


    if (modulus_type == X2X_MODULUS_POW2) {
        X2X_REGS->ctrl = X2X_CTRL_DATA_TYPE_POW2 | dual_flag;
    } else if (modulus_type == X2X_MODULUS_PRIME) {
        X2X_REGS->ctrl = X2X_CTRL_DATA_TYPE_PRIME | dual_flag;
    } else {
        return -1;
    }


    X2X_REGS->modulus = modulus[0];

    return 0;
}


int x2x_seed(uint32_t *seed)
{

    if ((X2X_REGS->ctrl & X2X_CTRL_BUSY_V)) {
        return -1;
    }

    X2X_REGS->seed[0] = seed[0];
    X2X_REGS->seed[1] = seed[1];

    return 0;
}


static int x2x_core(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len, uint32_t conv_mode, uint32_t mask)
{
    if ((X2X_REGS->ctrl & X2X_CTRL_BUSY_V)) {
        return -1;
    }

    X2X_REGS->data_len = len;
    X2X_REGS->din_addr[0] = (uint32_t) src_0;
    if (mask != X2X_CTRL_SRC_MASK_V) {
        X2X_REGS->din_addr[1] = (uint32_t) src_1;
    }
    X2X_REGS->dout_addr[0] = (uint32_t) dst_0;
    X2X_REGS->dout_addr[1] = (uint32_t) dst_1;

    X2X_REGS->ctrl = X2X_CTRL_START_V | conv_mode | mask;

    while ((X2X_REGS->ctrl & X2X_CTRL_DONE_V) == 0);

    return 0;
}


int x2x_a2b(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len)
{
    return x2x_core(dst_1, dst_0, src_1, src_0, len, X2X_CTRL_CONV_MODE_A2B, 0);
}


int x2x_b2a(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len)
{
    return x2x_core(dst_1, dst_0, src_1, src_0, len, X2X_CTRL_CONV_MODE_B2A, 0);
}


int x2x_b_mask(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src, unsigned int len)
{
    return x2x_core(dst_1, dst_0, 0x0, src, len, X2X_CTRL_CONV_MODE_A2B, X2X_CTRL_SRC_MASK_V);
}


int x2x_a_mask(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src, unsigned int len)
{
    return x2x_core(dst_1, dst_0, 0x0, src, len, X2X_CTRL_CONV_MODE_B2A, X2X_CTRL_SRC_MASK_V);
}
