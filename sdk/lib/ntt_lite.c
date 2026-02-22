#include "ntt_lite.h"
#include "risqrypt.h"
#include "sdk_benchmark.h"


unsigned int ntt_lite_cc = 0;


int ntt_lite_load_q(uint32_t q, const uint32_t *mu, uint32_t logn, uint32_t k, uint32_t inv2, uint32_t mode) {

    uint32_t mode_int;

    BENCH_START(ntt_lite_cc);

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

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
    NTT_LITE_REGS->mu[1] = mu[1];

    NTT_LITE_REGS->inv2 = inv2;

    NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | (k << NTT_LITE_CTRL_K_S) | mode_int;

    BENCH_END(ntt_lite_cc);
    return 0;
}


int ntt_lite_set_ctrl(uint32_t logn, uint32_t k, uint32_t mode) {

    uint32_t mode_int;

    BENCH_START(ntt_lite_cc);

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

    NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | (k << NTT_LITE_CTRL_K_S) | mode_int;

    BENCH_END(ntt_lite_cc);
    return 0;
}


int ntt_lite_set_q(uint32_t q) {
    BENCH_START(ntt_lite_cc);

    NTT_LITE_REGS->q = q;

    BENCH_END(ntt_lite_cc);
    return 0;
}


int ntt_lite_set_inv2(uint32_t inv2) {
    BENCH_START(ntt_lite_cc);

    NTT_LITE_REGS->inv2 = inv2;

    BENCH_END(ntt_lite_cc);
    return 0;
}


int ntt_lite_set_mu(const uint32_t *mu, uint32_t mode) {
    BENCH_START(ntt_lite_cc);

    NTT_LITE_REGS->mu[0] = mu[0];
    NTT_LITE_REGS->mu[1] = mu[1];

    BENCH_END(ntt_lite_cc);
    return 0;
}


int ntt_lite_set_bound(uint32_t bound) {
    BENCH_START(ntt_lite_cc);

    NTT_LITE_REGS->bound = bound;

    BENCH_END(ntt_lite_cc);
    return 0;
}


int ntt_lite_set_clr() {
    BENCH_START(ntt_lite_cc);

    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_SET_CLR;
    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    BENCH_END(ntt_lite_cc);
    return 0;
}


int ntt_lite_set_clr_with_twiddle() {
    BENCH_START(ntt_lite_cc);

    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_SET_CLR | NTT_LITE_CTRL_OP_SWITCH_EN_V;
    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    BENCH_END(ntt_lite_cc);
    return 0;
}


static int ntt_lite_load_twiddle_core_op(const uint32_t *psi, uint32_t op, uint32_t op_switch) {

    NTT_LITE_REGS->din_addr = (uint32_t) psi;
    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_TWIDDLE | op | op_switch;
    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    return 0;
}


static int ntt_lite_load_twiddle_core(const uint32_t *psi) {

    NTT_LITE_REGS->din_addr = (uint32_t) psi;
    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_TWIDDLE;
    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));
    return 0;
}


int ntt_lite_load_twiddle(const uint32_t *psi) {
    int ret;

    BENCH_START(ntt_lite_cc);

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    ret = ntt_lite_load_twiddle_core(psi);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_load_zeta(const uint32_t *zeta) {
    BENCH_START(ntt_lite_cc);

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    NTT_LITE_REGS->din_addr = (uint32_t) zeta;
    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_ZETA;
    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    BENCH_END(ntt_lite_cc);
    return 0;
}


static int ntt_lite_ntt_core(uint32_t *dst, const uint32_t *src, uint32_t op) {

    uint32_t cmd;
    uint32_t out_dis;

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    if (src == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) src;
    }

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;

    NTT_LITE_REGS->ctrl |= cmd | op;

    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    return 0;
}


int ntt_lite_forward_ntt(uint32_t *dst, const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_ntt_core(dst, src, NTT_LITE_CTRL_OP_NTT);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_backward_ntt(uint32_t *dst, const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_ntt_core(dst, src, NTT_LITE_CTRL_OP_INTT);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_mac(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    int ret;
    BENCH_START(ntt_lite_cc);

    if (lhs == NTT_LITE_INPUT_DIS) {
        return -1;
    }

    if (rhs != NTT_LITE_INPUT_DIS) {
        ntt_lite_load_twiddle_core(rhs);
    }

    ret = ntt_lite_ntt_core(dst, lhs, NTT_LITE_CTRL_OP_MAC);
    BENCH_END(ntt_lite_cc);
    return ret;
}


static int ntt_lite_pointwise_op(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs, uint32_t op, uint32_t rhs_const, uint32_t op_switch) {
    
    uint32_t cmd;
    uint32_t out_dis;

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;

    if (rhs != NTT_LITE_INPUT_DIS) {
        if ((lhs == NTT_LITE_INPUT_DIS) && ((op == NTT_LITE_CTRL_OP_ADD) || (op == NTT_LITE_CTRL_OP_SUB) || (op == NTT_LITE_CTRL_OP_PWM))) {
            return ntt_lite_load_twiddle_core_op(rhs, op, op_switch);
        }
        else {
            ntt_lite_load_twiddle_core(rhs);
        }
    }

    if (lhs == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) lhs;
    }

    NTT_LITE_REGS->ctrl |= cmd | op | rhs_const | op_switch;

    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    return 0;
}


int ntt_lite_pwm(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_PWM, 0, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_mul_const(uint32_t *dst, const uint32_t *lhs) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, lhs, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_PWM, NTT_LITE_CTRL_RHS_CONST_EN_V, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_add(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_ADD, 0, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_add_const(uint32_t *dst, const uint32_t *lhs) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, lhs, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_ADD, NTT_LITE_CTRL_RHS_CONST_EN_V, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_sub(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_SUB, 0, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_sub_const(uint32_t *dst, const uint32_t *lhs) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, lhs, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_SUB, NTT_LITE_CTRL_RHS_CONST_EN_V, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_sub_rev(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_SUB, 0, NTT_LITE_CTRL_OP_SWITCH_EN_V);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_sub_rev_const(uint32_t *dst, const uint32_t *lhs) { 
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, lhs, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_SUB, NTT_LITE_CTRL_RHS_CONST_EN_V, NTT_LITE_CTRL_OP_SWITCH_EN_V);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_sum(uint32_t *dst, const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, src, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_SUM, 0, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_sq(uint32_t *dst, const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, src, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_SQ, 0, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_sqadd(uint32_t *dst, const uint32_t *lhs, const uint32_t *rhs) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, lhs, rhs, NTT_LITE_CTRL_OP_SQADD, 0, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_tocenter(uint32_t *dst, const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, src, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_TOCENTER, 0, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_fromcenter(uint32_t *dst, const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, src, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_FROMCENTER, 0, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_encode(uint32_t *dst, const uint32_t *src, uint32_t d) {
    BENCH_START(ntt_lite_cc);

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;
    if (src != NTT_LITE_INPUT_DIS) {
        NTT_LITE_REGS->din_addr = (uint32_t) src;
        NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_POLY | NTT_LITE_CTRL_OP_ENCODE | (d << NTT_LITE_CTRL_D_S);
    } else {
        NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_START     | NTT_LITE_CTRL_OP_ENCODE | (d << NTT_LITE_CTRL_D_S);
    }

    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    BENCH_END(ntt_lite_cc);
    return 0;
}


static int ntt_lite_decode_core(uint32_t *dst, const uint32_t *src, uint32_t d, uint32_t op, uint32_t bigend) {

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    NTT_LITE_REGS->din_addr = (uint32_t) src;
    NTT_LITE_REGS->dout_addr = (uint32_t) dst;
    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_TWIDDLE | (d << NTT_LITE_CTRL_D_S) | op | bigend;
    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    return 0;
}


int ntt_lite_decode(uint32_t *dst, const uint32_t *src, uint32_t d) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_decode_core(dst, src, d, NTT_LITE_CTRL_OP_DECODE, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_decode_bigend(uint32_t *dst, const uint32_t *src, uint32_t d) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_decode_core(dst, src, d, NTT_LITE_CTRL_OP_DECODE, NTT_LITE_CTRL_BIGEND_EN_V);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_fndecompress(uint32_t *dst, const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_decode_core(dst, src, 0, NTT_LITE_CTRL_OP_FNDECOMPRESS, NTT_LITE_CTRL_BIGEND_EN_V);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_cbd(uint32_t *dst, const uint32_t *src, uint32_t d) {
    int ret;
    BENCH_START(ntt_lite_cc);
#ifndef BUSY_CHECK_DIS
    if ((d != 2) && (d != 3)) {
        return -1;
    }
#endif

    ret = ntt_lite_decode_core(dst, src, d << 1, NTT_LITE_CTRL_OP_CBD, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


static int ntt_lite_rejsamp_core(uint32_t *dst, const uint32_t *src, uint32_t d, uint32_t center, uint32_t bigend) {
    uint32_t center_int;
    uint32_t status;
    int ret;
    BENCH_START(ntt_lite_cc);

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    if (center == NTT_LITE_REJSAMP_CENTER_EN) {
        center_int = NTT_LITE_CTRL_RHS_CONST_EN_V;
    } else {
        center_int = 0;
    }

    NTT_LITE_REGS->din_addr = (uint32_t) src;
    NTT_LITE_REGS->dout_addr = (uint32_t) dst;
    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_TWIDDLE | NTT_LITE_CTRL_OP_REJSAMP | (d << NTT_LITE_CTRL_D_S) | center_int | bigend;

    do {
        status = NTT_LITE_REGS->status;
    }
    while(!(status & NTT_LITE_STATUS_DONE_V));

    if (status & NTT_LITE_STATUS_REJSAMP_IP_V) {
        ret = NTT_LITE_REJSAMP_IP;
    } else {
        ret = NTT_LITE_REJSAMP_DONE;
    }

    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_rejsamp(uint32_t *dst, const uint32_t *src, uint32_t d, uint32_t center) {
    return ntt_lite_rejsamp_core(dst, src, d, center, 0);
}


int ntt_lite_rejsamp_bigend(uint32_t *dst, const uint32_t *src, uint32_t d, uint32_t center) {
    return ntt_lite_rejsamp_core(dst, src, d, center, NTT_LITE_CTRL_BIGEND_EN_V);
}


int ntt_lite_compress(uint32_t *dst, const uint32_t *src, uint32_t d) {

    uint32_t cmd;
    uint32_t out_dis;
    BENCH_START(ntt_lite_cc);
#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    if (src == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) src;
    }

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;

    NTT_LITE_REGS->ctrl |= cmd | NTT_LITE_CTRL_OP_COMPRESS | (d << NTT_LITE_CTRL_D_S);
    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));
    BENCH_END(ntt_lite_cc);
    return 0;
}


static int ntt_lite_decompress_core(uint32_t *dst, const uint32_t *src, uint32_t d, uint32_t round) {

    uint32_t cmd;
    uint32_t out_dis;
    uint32_t round_dis;

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    if (src == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) src;
    }

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;

    NTT_LITE_REGS->ctrl |= cmd | NTT_LITE_CTRL_OP_DECOMPRESS | (d << NTT_LITE_CTRL_D_S) | round;
    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    return 0;
}


int ntt_lite_decompress(uint32_t *dst, const uint32_t *src, uint32_t d) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_decompress_core(dst, src, d, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_decompress_floor(uint32_t *dst, const uint32_t *src, uint32_t d) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_decompress_core(dst, src, d, NTT_LITE_CTRL_ROUND_DIS_V);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_decompose(uint32_t *dst_1, uint32_t *dst_0, const uint32_t *src) {
    BENCH_START(ntt_lite_cc);
    if (ntt_lite_pointwise_op(dst_0, src, NTT_LITE_INPUT_DIS, NTT_LITE_CTRL_OP_DECOMPOSE, 0, 0) != 0) {
        return -1;
    }

    if (dst_1 != NTT_LITE_OUTPUT_DIS) {
        NTT_LITE_REGS->dout_addr = (uint32_t) dst_1;
        NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_READ_TWIDDLE;
        while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));
    }

    BENCH_END(ntt_lite_cc);
    return 0;
}


static int ntt_lite_read_core(uint32_t *dst, uint32_t cmd) {
    BENCH_START(ntt_lite_cc);

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    NTT_LITE_REGS->dout_addr = (uint32_t) dst;
    NTT_LITE_REGS->ctrl |= cmd;
    while(!(NTT_LITE_REGS->status & NTT_LITE_STATUS_DONE_V));

    BENCH_END(ntt_lite_cc);
    return 0;
}


int ntt_lite_read_twiddle(uint32_t *dst) {
    return ntt_lite_read_core(dst, NTT_LITE_CTRL_CMD_READ_TWIDDLE);
}


int ntt_lite_read_poly(uint32_t *dst) {
    return ntt_lite_read_core(dst, NTT_LITE_CTRL_CMD_READ_POLY);
}


static int ntt_lite_chknorm(const uint32_t *src, uint32_t op) {
    
    uint32_t cmd;
    uint32_t status;
    int ret;

#ifndef BUSY_CHECK_DIS
    if ((NTT_LITE_REGS->status & NTT_LITE_STATUS_BUSY_V)) {
        return -1;
    }
#endif

    if (src == NTT_LITE_INPUT_DIS) {
        cmd = NTT_LITE_CTRL_CMD_START;
    } else {
        cmd = NTT_LITE_CTRL_CMD_LOAD_POLY;
        NTT_LITE_REGS->din_addr = (uint32_t) src;
    }

    NTT_LITE_REGS->ctrl |= cmd | op;

    do {
        status = NTT_LITE_REGS->status;
    } while(!(status & NTT_LITE_STATUS_DONE_V));

    if (status & NTT_LITE_STATUS_CHKNORM_V) {
        ret = NTT_LITE_CHKNORM_FAIL;
    } else {
        ret = NTT_LITE_CHKNORM_SUCC;
    }
    return ret;
}


int ntt_lite_chkinfnorm(const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_chknorm(src, NTT_LITE_CTRL_OP_CHKINFNORM);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_chkl1norm(const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_chknorm(src, NTT_LITE_CTRL_OP_CHKL1NORM);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_make_hint(uint32_t *dst, const uint32_t *src_0, const uint32_t *src_1) {
    int ret;
    BENCH_START(ntt_lite_cc);
    ret = ntt_lite_pointwise_op(dst, src_0, src_1, NTT_LITE_CTRL_OP_MAKEHINT, 0, 0);
    BENCH_END(ntt_lite_cc);
    return ret;
}


int ntt_lite_use_hint(uint32_t *dst, const uint32_t *src) {
    int ret;
    BENCH_START(ntt_lite_cc);

    if (src == NTT_LITE_INPUT_DIS) {
        return -1;
    }

    ret = ntt_lite_ntt_core(dst, src, NTT_LITE_CTRL_OP_USEHINT);
    BENCH_END(ntt_lite_cc);
    return ret;
}