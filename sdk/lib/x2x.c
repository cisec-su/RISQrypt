#include "x2x.h"
#include "hornet.h"



int x2x_set_modulus(uint32_t modulus, uint32_t log_modulus, uint32_t modulus_type, uint32_t dual_mode, uint32_t rej_sample)
{
    uint32_t dual_flag;
    uint32_t rej_flag;
    uint32_t logm_flag;


    if (dual_mode == X2X_DUAL_MODE_EN) {
        dual_flag = X2X_CTRL_DUAL_MODE_EN_V;
    } else if (dual_mode == X2X_DUAL_MODE_DIS) {
        dual_flag = 0;
    } else {
        return -1;
    }

    if (rej_sample == X2X_REJ_SAMPLE_EN) {
        rej_flag = 0;
    } else if (rej_sample == X2X_REJ_SAMPLE_DIS) {
        rej_flag = X2X_CTRL_REJS_DIS_V;
    } else {
        return -1;
    }

    logm_flag = (log_modulus & X2X_CTRL_LOG_MODULUS_M) << X2X_CTRL_LOG_MODULUS_S;
    

    if (modulus_type == X2X_MODULUS_POW2) {
        X2X_REGS->ctrl = X2X_CTRL_DATA_TYPE_POW2 | dual_flag | logm_flag | rej_flag;
    } else if (modulus_type == X2X_MODULUS_PRIME) {
        X2X_REGS->ctrl = X2X_CTRL_DATA_TYPE_PRIME | dual_flag | logm_flag | rej_flag;
    } else {
        return -1;
    }


    X2X_REGS->modulus = modulus;

    return 0;
}


int x2x_seed(uint32_t seed[2])
{

    if ((X2X_REGS->ctrl & X2X_CTRL_BUSY_V)) {
        return -1;
    }

    X2X_REGS->seed[0] = seed[0];
    X2X_REGS->seed[1] = seed[1];

    return 0;
}


static int x2x_core(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len, uint32_t conv_mode, uint32_t share, uint32_t b2a_1bit, uint32_t log_stride)
{
    uint32_t ctrl;
    ctrl = X2X_REGS->ctrl;
    if (ctrl & X2X_CTRL_BUSY_V) {
        return -1;
    }

    while (ctrl & X2X_CTRL_SEED_BUSY_V) {
        ctrl = X2X_REGS->ctrl;
    }

    X2X_REGS->data_len = len;
    X2X_REGS->din_addr[0] = (uint32_t) src_0;
    if (share != X2X_CTRL_SRC_SHARE_V) {
        X2X_REGS->din_addr[1] = (uint32_t) src_1;
    }
    X2X_REGS->dout_addr[0] = (uint32_t) dst_0;
    X2X_REGS->dout_addr[1] = (uint32_t) dst_1;

    X2X_REGS->ctrl |= X2X_CTRL_START_V | conv_mode | share | b2a_1bit | log_stride | X2X_CTRL_CMD_X2X;

    while (!(X2X_REGS->ctrl & X2X_CTRL_DONE_V));

    return 0;
}


int x2x_a2b(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len)
{
    return x2x_core(dst_1, dst_0, src_1, src_0, len, X2X_CTRL_CONV_MODE_A2B, 0, 0, 0);
}


int x2x_b2a(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, unsigned int len)
{
    return x2x_core(dst_1, dst_0, src_1, src_0, len, X2X_CTRL_CONV_MODE_B2A, 0, 0, 0);
}

int x2x_b2a_1bit(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src_1, uint32_t *src_0, uint32_t log_stride, unsigned int len)
{
    return x2x_core(dst_1, dst_0, src_1, src_0, len, X2X_CTRL_CONV_MODE_B2A, 0, X2X_CTRL_B2A_BIT_EN_V, (log_stride << X2X_CTRL_LOG_STRIDE_S));
}


int x2x_b_share(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src, unsigned int len)
{
    return x2x_core(dst_1, dst_0, 0x0, src, len, X2X_CTRL_CONV_MODE_A2B, X2X_CTRL_SRC_SHARE_V, 0, 0);
}


int x2x_a_share(uint32_t *dst_1, uint32_t *dst_0, uint32_t *src, unsigned int len)
{
    return x2x_core(dst_1, dst_0, 0x0, src, len, X2X_CTRL_CONV_MODE_B2A, X2X_CTRL_SRC_SHARE_V, 0, 0);
}
