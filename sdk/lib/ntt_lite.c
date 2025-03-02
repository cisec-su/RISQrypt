#include "ntt_lite_regs.h"
#include "ntt_lite.h"
#include "hornet.h"



int ntt_lite_load_q(const uint32_t *q, const uint32_t *mu, unsigned int logn, unsigned int logq, unsigned int mode) {

    unsigned int mode_int;


    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    if (logn > 8) {
        return -1;
    }

    if (logq > 32) {
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

    NTT_LITE_REGS->q = q[0];
    NTT_LITE_REGS->mu[0] = mu[0];

    if (logq == 32) {
        NTT_LITE_REGS->mu[1] = mu[1];
    }

    NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | (logq << NTT_LITE_CTRL_LOGQ_S) | mode_int;

    return 0;
}


int ntt_lite_load_twiddle(const uint32_t *psi) {

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    NTT_LITE_REGS->din_addr = (uint32_t) psi;
    NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_TWIDDLE;
    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    return 0;
}


static int ntt_lite_core(uint32_t *dst, const uint32_t *src, int flag) {

    if ((NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_BUSY_V)) {
        return -1;
    }

    NTT_LITE_REGS->stride = 1;
    NTT_LITE_REGS->din_addr = (uint32_t) src;
    NTT_LITE_REGS->dout_addr = (uint32_t) dst;
    if (flag) {
        NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_POLY | NTT_LITE_CTRL_OP_NTT;
    } else {
        NTT_LITE_REGS->ctrl |= NTT_LITE_CTRL_CMD_LOAD_POLY | NTT_LITE_CTRL_OP_INTT;
    }

    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    return 0;
}


int ntt_lite_forward_ntt(uint32_t *dst, const uint32_t *src) {
    return ntt_lite_core(dst, src, 1);
}


int ntt_lite_backward_ntt(uint32_t *dst, const uint32_t *src) {
    return ntt_lite_core(dst, src, 0);
}



// int ntt_lite_forward_2d(uint32_t *b,  uint32_t *a, uint32_t *w0, uint32_t *w1, uint32_t *q, unsigned int logn, unsigned int logn0, unsigned int logq) {

//     unsigned int n = 1 << logn;
//     unsigned int n0 = 1 << logn0;
//     unsigned int logn1 = logn - logn0;
//     unsigned int n1 = 1 << logn1;
//     unsigned int i;
//     unsigned int q_shift;


//     NTT_LITE_REGS->din_addr = (uint32_t) q;
//     if (logq == 64) {
//         NTT_LITE_REGS->ctrl = NTT_LITE_CTRL_LOGQ_64 | NTT_LITE_CTRL_CMD_LOAD_Q;
//         q_shift = sizeof(uint64_t);
//     } else {
//         NTT_LITE_REGS->ctrl = NTT_LITE_CTRL_LOGQ_32 | NTT_LITE_CTRL_CMD_LOAD_Q;
//         q_shift = sizeof(uint32_t);
//     }
//     while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

//     NTT_LITE_REGS->din_addr = (uint32_t) w0;
//     NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_CMD_LOAD_TWIDDLE;
//     while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

//     NTT_LITE_REGS->stride = n1;
//     for (i = 0; i < n1; i++) {
//         NTT_LITE_REGS->din_addr = (uint32_t) (a + (i << q_shift));
//         NTT_LITE_REGS->dout_addr = (uint32_t) (b + (i << q_shift));
//         NTT_LITE_REGS->ctrl = (logn0 << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_MODE_NTT | NTT_LITE_CTRL_CMD_LOAD_POLY | NTT_LITE_CTRL_OTF_DIS;
//         while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));
//     }

//     NTT_LITE_REGS->din_addr = (uint32_t) w1;
//     NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_CMD_LOAD_TWIDDLE | NTT_LITE_CTRL_OTF_EN;
//     while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

//     NTT_LITE_REGS->stride = 1;
//     for (i = 0; i < n0; i++) {
//         NTT_LITE_REGS->din_addr = (uint32_t) (b + ((i << logn1) << q_shift));
//         NTT_LITE_REGS->dout_addr = (uint32_t) (b + ((i << logn1) << q_shift));
//         NTT_LITE_REGS->ctrl = (logn1 << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_MODE_NTT | NTT_LITE_CTRL_CMD_LOAD_POLY | NTT_LITE_CTRL_OTF_DIS;
//         while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));
//     }

//     return 0;
// }