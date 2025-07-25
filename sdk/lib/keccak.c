#include "keccak.h"
#include "hornet.h"



int keccak_init(uint32_t rate, uint32_t mask) {

    uint32_t mask_flag;   

    if (rate > 25) {
        return -1;
    }
    
    if (mask == KECCAK_MASK_EN) {
        mask_flag = 0;
    }
    else if (mask == KECCAK_MASK_DIS) {
        mask_flag = KECCAK_CTRL_S_DIS_1S;
    }
    else {
        return -1;
    }

    KECCAK_REGS->ctrl = KECCAK_CTRL_CMD_RST;
    while(!(KECCAK_REGS->ctrl & KECCAK_CTRL_DONE_V));
    KECCAK_REGS->ctrl = (rate << KECCAK_CTRL_RATE_S) | mask_flag;

    return 0;
}


int keccak_absorb(const uint32_t *share_0, const uint32_t *share_1, unsigned int len) {

    KECCAK_REGS->data_len = len;
    KECCAK_REGS->din_addr[0] = (uint32_t) share_0;
    KECCAK_REGS->din_addr[1] = (uint32_t) share_1;

    KECCAK_REGS->ctrl |= KECCAK_CTRL_CMD_ABSORB;

    while(!(KECCAK_REGS->ctrl & KECCAK_CTRL_DONE_V));

    return 0;
}


int keccak_absorb_public(const uint32_t *src, unsigned int len) {

    KECCAK_REGS->data_len = len;
    KECCAK_REGS->din_addr[0] = (uint32_t) src;

    KECCAK_REGS->ctrl |= KECCAK_CTRL_CMD_ABSORB | KECCAK_CTRL_S_DIS_1S;

    while(!(KECCAK_REGS->ctrl & KECCAK_CTRL_DONE_V));

    KECCAK_REGS->ctrl &= ~KECCAK_CTRL_S_DIS_V;

    return 0;
}

int keccak_finish(const uint32_t *pad_word) {

    if (pad_word != KECCAK_NULL_PAD_WORD) {
        uint32_t temp = KECCAK_REGS->ctrl;
        KECCAK_REGS->din_addr[0] = (uint32_t) pad_word;
        KECCAK_REGS->data_len = 1;

        KECCAK_REGS->ctrl |= KECCAK_CTRL_CMD_ABSORB | KECCAK_CTRL_PERM_DIS_V | KECCAK_CTRL_S_DIS_1S;
        while(!(KECCAK_REGS->ctrl & KECCAK_CTRL_DONE_V));
        KECCAK_REGS->ctrl = temp;
    }
    KECCAK_REGS->ctrl |= KECCAK_CTRL_CMD_PAD;
    while(!(KECCAK_REGS->ctrl & KECCAK_CTRL_DONE_V));

    return 0;
}


int keccak_squeeze(uint32_t *share_0, uint32_t *share_1, unsigned int len) {

    KECCAK_REGS->data_len = len;
    KECCAK_REGS->dout_addr[0] = (uint32_t) share_0;
    KECCAK_REGS->dout_addr[1] = (uint32_t) share_1;

    KECCAK_REGS->ctrl |= KECCAK_CTRL_CMD_SQUEEZE;

    while(!(KECCAK_REGS->ctrl & KECCAK_CTRL_DONE_V));

    return 0;
}