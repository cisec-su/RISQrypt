#include "ntt_lite.h"
#include "hornet.h"



int ntt_lite_load_q(uint32_t q, const uint32_t *mu, uint32_t logn, uint32_t k, uint32_t inv2, uint32_t mode) {

    uint32_t mode_int;

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    if (logn > 8) {
        return -1;
    }

    if (k > 32) {
        return -1;
    }

    if (mode == NTT_LITE_MODE_SINGLE) {
        mode_int = NTT_LITE_CTRL_MODE_SINGLE;
    } else if (mode == NTT_LITE_MODE_DUAL) {
        mode_int = NTT_LITE_CTRL_MODE_DUAL;
    } else if (mode == NTT_LITE_MODE_POLY) {
        mode_int = NTT_LITE_CTRL_MODE_POLY;
    } else {
        return -1;
    }

    NTT_LITE_REGS->q = q;
    NTT_LITE_REGS->mu[0] = mu[0];
    if (mode == NTT_LITE_MODE_SINGLE) {
        NTT_LITE_REGS->mu[1] = mu[1];
    }

    NTT_LITE_REGS->inv2 = inv2;

    NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | (k << NTT_LITE_CTRL_K_S) | mode_int;

    return 0;
}


int ntt_lite_set_q(uint32_t q) {

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    NTT_LITE_REGS->q = q;

    return 0;
}


int ntt_lite_set_inv2(uint32_t inv2) {

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    NTT_LITE_REGS->inv2 = inv2;

    return 0;
}


int ntt_lite_set_mu(const uint32_t *mu, uint32_t mode) {
    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    NTT_LITE_REGS->mu[0] = mu[0];
    if (mode == NTT_LITE_MODE_SINGLE) {
        NTT_LITE_REGS->mu[1] = mu[1];
    }
    return 0;
}


int ntt_lite_set_bound(uint32_t bound) {

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    NTT_LITE_REGS->bound = bound;

    return 0;
}


int ntt_lite_set_mode(uint32_t mode) {

    uint32_t mode_int;

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    if (mode == NTT_LITE_MODE_SINGLE) {
        mode_int = NTT_LITE_CTRL_MODE_SINGLE;
    } else if (mode == NTT_LITE_MODE_DUAL) {
        mode_int = NTT_LITE_CTRL_MODE_DUAL;
    } else if (mode == NTT_LITE_MODE_POLY) {
        mode_int = NTT_LITE_CTRL_MODE_POLY;
    } else {
        return -1;
    }

    NTT_LITE_REGS->ctrl = (NTT_LITE_REGS->ctrl & ~NTT_LITE_CTRL_MODE_V) | mode_int;

    return 0;
}


static int ntt_lite_load_twiddle_core(const uint32_t *psi, uint32_t rhs_const) {

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    NTT_LITE_REGS->din_addr = (uint32_t) psi;
    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_TWIDDLE | rhs_const;
    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    return 0;
}


int ntt_lite_load_twiddle(const uint32_t *psi) {
    return ntt_lite_load_twiddle_core(psi, 0);
}


static int ntt_lite_ntt_core(uint32_t *dst, const uint32_t *src, uint32_t op) {

    uint32_t cmd;
    uint32_t out_dis;

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    if (src == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) src;
    }

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;

    NTT_LITE_REGS->ctrl |= cmd | op;

    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    return 0;
}


int ntt_lite_forward_ntt(uint32_t *dst, const uint32_t *src) {
    return ntt_lite_ntt_core(dst, src, NTT_LITE_CTRL_OP_NTT);
}


int ntt_lite_backward_ntt(uint32_t *dst, const uint32_t *src) {
    return ntt_lite_ntt_core(dst, src, NTT_LITE_CTRL_OP_INTT);
}


static int ntt_lite_pointwise_op(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs, uint32_t op, uint32_t rhs_const, uint32_t op_switch) {
    
    uint32_t cmd;
    uint32_t out_dis;

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    if (rhs != NTT_LITE_INPUT_DIS) {
        ntt_lite_load_twiddle_core(rhs, rhs_const);
    }

    if (lhs == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) lhs;
    }

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;

    NTT_LITE_REGS->ctrl |= cmd | op | rhs_const | op_switch;

    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    return 0;
}


int ntt_lite_pwm(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    return ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_PWM, 0, 0);
}


int ntt_lite_mul_const(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    return ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_PWM, NTT_LITE_CTRL_RHS_CONST_EN_V, 0);
}


int ntt_lite_add(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    return ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_ADD, 0, 0);
}


int ntt_lite_add_const(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    return ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_ADD, NTT_LITE_CTRL_RHS_CONST_EN_V, 0);
}


int ntt_lite_sub(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    return ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_SUB, 0, 0);
}


int ntt_lite_sub_const(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    return ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_SUB, NTT_LITE_CTRL_RHS_CONST_EN_V, 0);
}


int ntt_lite_sub_rev(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    return ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_SUB, 0, NTT_LITE_CTRL_OP_SWITCH_EN_V);
}


int ntt_lite_sub_rev_const(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    return ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_SUB, NTT_LITE_CTRL_RHS_CONST_EN_V, NTT_LITE_CTRL_OP_SWITCH_EN_V);
}


int ntt_lite_sum(uint32_t *dst, const uint32_t *src) {
    return ntt_lite_pointwise_op(dst, src, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_SUM, 0, 0);
}


int ntt_lite_encode(uint32_t *dst, const uint32_t *src, uint32_t d) {

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;
    if (src != NTT_LITE_INPUT_DIS) {
        NTT_LITE_REGS->din_addr = (uint32_t) src;
        NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_POLY | NTT_LITE_CTRL_OP_ENCODE | (d << NTT_LITE_CTRL_D_S);
    } else {
        NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_START     | NTT_LITE_CTRL_OP_ENCODE | (d << NTT_LITE_CTRL_D_S);
    }

    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    return 0;
}


int ntt_lite_decode(uint32_t *dst, const uint32_t *src, uint32_t d) {

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    ntt_lite_load_twiddle(src);

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;
    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_START | NTT_LITE_CTRL_OP_DECODE | (d << NTT_LITE_CTRL_D_S);


    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    return 0;
}


int ntt_lite_compress(uint32_t *dst, const uint32_t *src, uint32_t d) {

    uint32_t cmd;
    uint32_t out_dis;

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    if (src == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) src;
    }

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;

    NTT_LITE_REGS->ctrl |= cmd | NTT_LITE_CTRL_OP_COMPRESS | (d << NTT_LITE_CTRL_D_S)/* | out_dis*/;
    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    return 0;
}


static int ntt_lite_decompress_core(uint32_t *dst, const uint32_t *src, uint32_t d, uint32_t round) {

    uint32_t cmd;
    uint32_t out_dis;
    uint32_t round_dis;

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    if (src == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) src;
    }

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;

    NTT_LITE_REGS->ctrl |= cmd | NTT_LITE_CTRL_OP_DECOMPRESS | (d << NTT_LITE_CTRL_D_S) | round;
    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));
}


int ntt_lite_decompress(uint32_t *dst, const uint32_t *src, uint32_t d) {
    return ntt_lite_decompress_core(dst, src, d, 0);
}


int ntt_lite_decompress_floor(uint32_t *dst, const uint32_t *src, uint32_t d) {
    return ntt_lite_decompress_core(dst, src, d, NTT_LITE_CTRL_ROUND_DIS_V);
}


int ntt_lite_decompose(uint32_t *dst_1, uint32_t *dst_0, const uint32_t *src) {
    if (ntt_lite_pointwise_op(dst_0, src, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_DECOMPOSE, 0, 0) != 0) {
        return -1;
    }

    if (dst_1 != NTT_LITE_OUTPUT_DIS) {
        NTT_LITE_REGS->dout_addr = (uint32_t) dst_1;
        NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_READ_TWIDDLE;
        while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));
    }

    return 0;
}    


int ntt_lite_chknorm(const uint32_t *src) {
    
    uint32_t cmd;
    uint32_t status;

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    if (src == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) src;
    }

    NTT_LITE_REGS->ctrl |= cmd | NTT_LITE_CTRL_OP_CHKNORM;

    do {
        status = NTT_LITE_REGS->ctrl;
    } while(!(status & NTT_LITE_CTRL_DONE_V));

    if (status & NTT_LITE_CTRL_CHKNORM_V) {
        return NTT_LITE_CHKNORM_FAIL;
    } else {
        return NTT_LITE_CHKNORM_SUCC;
    }
}



int ntt_lite_make_hint(uint32_t *dst, const uint32_t *src_0, const uint32_t *src_1) {
    return ntt_lite_pointwise_op(dst, src_0, src_1, NTT_LITE_CTRL_OP_MAKEHINT, 0, 0);
}
