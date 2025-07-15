#ifndef NTT_LITE_CTRL_H
#define NTT_LITE_CTRL_H


#include <stdint.h>


typedef volatile uint32_t IO;

typedef volatile uint32_t I;


typedef struct
{
    IO ctrl;       // BASE_ADDR + 0x00000000
    I  din_addr;   // BASE_ADDR + 0x00000004
    IO dout_addr;  // BASE_ADDR + 0x00000008
    IO bound;      // BASE_ADDR + 0x0000000C
    IO q;          // BASE_ADDR + 0x00000010
    IO mu[2];      // BASE_ADDR + 0x00000014
    IO inv2;       // BASE_ADDR + 0x0000001C
} ntt_lite_regs_t;



#define NTT_LITE_CTRL_CMD_S                ((uint32_t) 0  )
#define NTT_LITE_CTRL_CMD_M                ((uint32_t) 0xF)
#define NTT_LITE_CTRL_CMD_V                (NTT_LITE_CTRL_CMD_M << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_IDLE             (((uint32_t) 0x0) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_RST              (((uint32_t) 0x1) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_START            (((uint32_t) 0x2) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_LOAD_TWIDDLE     (((uint32_t) 0x3) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_LOAD_POLY        (((uint32_t) 0x4) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_READ_TWIDDLE     (((uint32_t) 0x5) << NTT_LITE_CTRL_CMD_S)

#define NTT_LITE_CTRL_LOGN_S               ((uint32_t) 4  )
#define NTT_LITE_CTRL_LOGN_M               ((uint32_t) 0xF)
#define NTT_LITE_CTRL_LOGN_V               (NTT_LITE_CTRL_LOGN_M << NTT_LITE_CTRL_LOGN_S)

#define NTT_LITE_CTRL_OP_S                 ((uint32_t) 8  )
#define NTT_LITE_CTRL_OP_M                 ((uint32_t) 0xF)
#define NTT_LITE_CTRL_OP_V                 (NTT_LITE_CTRL_OP_M << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_NTT               (((uint32_t) 0x0) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_INTT              (((uint32_t) 0x1) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_PWM               (((uint32_t) 0x2) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_ADD               (((uint32_t) 0x3) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_SUB               (((uint32_t) 0x4) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_COMPRESS          (((uint32_t) 0x5) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_DECOMPRESS        (((uint32_t) 0x6) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_ENCODE            (((uint32_t) 0x7) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_DECODE            (((uint32_t) 0x8) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_SUM               (((uint32_t) 0x9) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_DECOMPOSE         (((uint32_t) 0xA) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_CHKNORM           (((uint32_t) 0xB) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_MAKEHINT          (((uint32_t) 0xC) << NTT_LITE_CTRL_OP_S)

#define NTT_LITE_CTRL_OP_SWITCH_EN_S       ((uint32_t) 12 )
#define NTT_LITE_CTRL_OP_SWITCH_EN_M       ((uint32_t) 0x1)
#define NTT_LITE_CTRL_OP_SWITCH_EN_V       (NTT_LITE_CTRL_OP_SWITCH_EN_M << NTT_LITE_CTRL_OP_SWITCH_EN_S)

#define NTT_LITE_CTRL_K_S                  ((uint32_t) 13  )
#define NTT_LITE_CTRL_K_M                  ((uint32_t) 0x1F)
#define NTT_LITE_CTRL_K_V                  (NTT_LITE_CTRL_K_M << NTT_LITE_CTRL_K_M)

#define NTT_LITE_CTRL_MODE_S               ((uint32_t) 19  )
#define NTT_LITE_CTRL_MODE_M               ((uint32_t) 0x3 )
#define NTT_LITE_CTRL_MODE_V               (NTT_LITE_CTRL_MODE_M << NTT_LITE_CTRL_MODE_S)
#define NTT_LITE_CTRL_MODE_SINGLE          (((uint32_t) 0x0) << NTT_LITE_CTRL_MODE_S)
#define NTT_LITE_CTRL_MODE_DUAL            (((uint32_t) 0x2) << NTT_LITE_CTRL_MODE_S)
#define NTT_LITE_CTRL_MODE_POLY            (((uint32_t) 0x3) << NTT_LITE_CTRL_MODE_S)

#define NTT_LITE_CTRL_D_S                  ((uint32_t) 21  )
#define NTT_LITE_CTRL_D_M                  ((uint32_t) 0x1F)
#define NTT_LITE_CTRL_D_V                  (NTT_LITE_CTRL_D_M << NTT_LITE_CTRL_D_S)

#define NTT_LITE_CTRL_CHKNORM_S            ((uint32_t) 26 )
#define NTT_LITE_CTRL_CHKNORM_M            ((uint32_t) 0x1)
#define NTT_LITE_CTRL_CHKNORM_V            (NTT_LITE_CTRL_CHKNORM_M << NTT_LITE_CTRL_CHKNORM_S)

#define NTT_LITE_CTRL_RHS_CONST_EN_S       ((uint32_t) 27 )
#define NTT_LITE_CTRL_RHS_CONST_EN_M       ((uint32_t) 0x1)
#define NTT_LITE_CTRL_RHS_CONST_EN_V       (NTT_LITE_CTRL_RHS_CONST_EN_M << NTT_LITE_CTRL_RHS_CONST_EN_S)

#define NTT_LITE_CTRL_ROUND_DIS_S          ((uint32_t) 28 )
#define NTT_LITE_CTRL_ROUND_DIS_M          ((uint32_t) 0x1)
#define NTT_LITE_CTRL_ROUND_DIS_V          (NTT_LITE_CTRL_ROUND_DIS_M << NTT_LITE_CTRL_ROUND_DIS_S)

#define NTT_LITE_CTRL_BUSY_S               ((uint32_t) 30 )
#define NTT_LITE_CTRL_BUSY_M               ((uint32_t) 0x1)
#define NTT_LITE_CTRL_BUSY_V               (NTT_LITE_CTRL_BUSY_M << NTT_LITE_CTRL_BUSY_S)

#define NTT_LITE_CTRL_DONE_S               ((uint32_t) 31 )
#define NTT_LITE_CTRL_DONE_M               ((uint32_t) 0x1)
#define NTT_LITE_CTRL_DONE_V               (NTT_LITE_CTRL_DONE_M << NTT_LITE_CTRL_DONE_S)


#endif