#include "ntt_lite_regs.h"
#include "hornet.h"




int ntt_lite_forward(uint32_t *b,  uint32_t *a, uint32_t *w, uint32_t *q, unsigned int logn, unsigned int logq) {

    NTT_LITE_REGS->din_addr = (uint32_t) q;
    if (logq == 64) {
        NTT_LITE_REGS->ctrl = NTT_LITE_CTRL_LOGQ_64 | NTT_LITE_CTRL_CMD_LOAD_Q;
    } else {
        NTT_LITE_REGS->ctrl = NTT_LITE_CTRL_LOGQ_32 | NTT_LITE_CTRL_CMD_LOAD_Q;
    }
    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    NTT_LITE_REGS->din_addr = (uint32_t) w;
    NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_CMD_LOAD_TWIDDLE;
    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    NTT_LITE_REGS->stride = 1;
    NTT_LITE_REGS->din_addr = (uint32_t) a;
    NTT_LITE_REGS->dout_addr = (uint32_t) b;
    NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_MODE_NTT | NTT_LITE_CTRL_CMD_LOAD_POLY | NTT_LITE_CTRL_OTF_DIS;


    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    return 0;
}




int ntt_lite_forward_2d(uint32_t *b,  uint32_t *a, uint32_t *w0, uint32_t *w1, uint32_t *q, unsigned int logn, unsigned int logn0, unsigned int logq) {

    unsigned int n = 1 << logn;
    unsigned int n0 = 1 << logn0;
    unsigned int logn1 = logn - logn0;
    unsigned int n1 = 1 << logn1;
    unsigned int i;
    unsigned int q_shift;


    NTT_LITE_REGS->din_addr = (uint32_t) q;
    if (logq == 64) {
        NTT_LITE_REGS->ctrl = NTT_LITE_CTRL_LOGQ_64 | NTT_LITE_CTRL_CMD_LOAD_Q;
        q_shift = sizeof(uint64_t);
    } else {
        NTT_LITE_REGS->ctrl = NTT_LITE_CTRL_LOGQ_32 | NTT_LITE_CTRL_CMD_LOAD_Q;
        q_shift = sizeof(uint32_t);
    }
    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    NTT_LITE_REGS->din_addr = (uint32_t) w0;
    NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_CMD_LOAD_TWIDDLE;
    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    NTT_LITE_REGS->stride = n1;
    for (i = 0; i < n1; i++) {
        NTT_LITE_REGS->din_addr = (uint32_t) (a + (i << q_shift));
        NTT_LITE_REGS->dout_addr = (uint32_t) (b + (i << q_shift));
        NTT_LITE_REGS->ctrl = (logn0 << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_MODE_NTT | NTT_LITE_CTRL_CMD_LOAD_POLY | NTT_LITE_CTRL_OTF_DIS;
        while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));
    }

    NTT_LITE_REGS->din_addr = (uint32_t) w1;
    NTT_LITE_REGS->ctrl = (logn << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_CMD_LOAD_TWIDDLE | NTT_LITE_CTRL_OTF_EN;
    while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));

    NTT_LITE_REGS->stride = 1;
    for (i = 0; i < n0; i++) {
        NTT_LITE_REGS->din_addr = (uint32_t) (b + ((i << logn1) << q_shift));
        NTT_LITE_REGS->dout_addr = (uint32_t) (b + ((i << logn1) << q_shift));
        NTT_LITE_REGS->ctrl = (logn1 << NTT_LITE_CTRL_LOGN_S) | NTT_LITE_CTRL_MODE_NTT | NTT_LITE_CTRL_CMD_LOAD_POLY | NTT_LITE_CTRL_OTF_DIS;
        while(!(NTT_LITE_REGS->ctrl & NTT_LITE_CTRL_DONE_V));
    }

    return 0;
}