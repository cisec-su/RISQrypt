#ifndef NTT_LITE_CTRL_H
#define NTT_LITE_CTRL_H


#include <stdint.h>


typedef volatile uint32_t IO;

typedef volatile uint32_t I;


typedef struct
{
    IO ctrl;       // BASE_ADDR + 0x00000000
    I  status;     // BASE_ADDR + 0x00000004
    I  din_addr;   // BASE_ADDR + 0x00000008
    IO dout_addr;  // BASE_ADDR + 0x0000000C
    IO bound;      // BASE_ADDR + 0x00000010
    IO q;          // BASE_ADDR + 0x00000014
    IO mu[2];      // BASE_ADDR + 0x0000001C
    IO inv2;       // BASE_ADDR + 0x00000020
} ntt_lite_regs_t;



#define NTT_LITE_CTRL_CMD_S                ((uint32_t) 0  )
#define NTT_LITE_CTRL_CMD_M                ((uint32_t) 0x7)
#define NTT_LITE_CTRL_CMD_V                (NTT_LITE_CTRL_CMD_M << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_IDLE             (((uint32_t) 0x0) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_SET_CLR          (((uint32_t) 0x1) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_START            (((uint32_t) 0x2) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_LOAD_TWIDDLE     (((uint32_t) 0x3) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_LOAD_POLY        (((uint32_t) 0x4) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_READ_TWIDDLE     (((uint32_t) 0x5) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_LOAD_ZETA        (((uint32_t) 0x6) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_READ_POLY        (((uint32_t) 0x7) << NTT_LITE_CTRL_CMD_S)

#define NTT_LITE_CTRL_LOGN_S               ((uint32_t) 3  )
#define NTT_LITE_CTRL_LOGN_M               ((uint32_t) 0xF)
#define NTT_LITE_CTRL_LOGN_V               (NTT_LITE_CTRL_LOGN_M << NTT_LITE_CTRL_LOGN_S)

#define NTT_LITE_CTRL_OP_S                 ((uint32_t) 7   )
#define NTT_LITE_CTRL_OP_M                 ((uint32_t) 0x1F)
#define NTT_LITE_CTRL_OP_V                 (NTT_LITE_CTRL_OP_M << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_NTT               (((uint32_t) 0x0 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_INTT              (((uint32_t) 0x1 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_PWM               (((uint32_t) 0x2 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_ADD               (((uint32_t) 0x3 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_SUB               (((uint32_t) 0x4 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_COMPRESS          (((uint32_t) 0x5 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_DECOMPRESS        (((uint32_t) 0x6 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_ENCODE            (((uint32_t) 0x7 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_DECODE            (((uint32_t) 0x8 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_SUM               (((uint32_t) 0x9 ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_DECOMPOSE         (((uint32_t) 0xA ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_CHKINFNORM        (((uint32_t) 0xB ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_MAKEHINT          (((uint32_t) 0xC ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_CBD               (((uint32_t) 0xD ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_REJSAMP           (((uint32_t) 0xE ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_MAC               (((uint32_t) 0xF ) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_USEHINT           (((uint32_t) 0x10) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_CHKL1NORM         (((uint32_t) 0x11) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_SQ                (((uint32_t) 0x12) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_SQADD             (((uint32_t) 0x13) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_TOCENTER          (((uint32_t) 0x14) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_FROMCENTER        (((uint32_t) 0x15) << NTT_LITE_CTRL_OP_S)
#define NTT_LITE_CTRL_OP_FNDECOMPRESS      (((uint32_t) 0x16) << NTT_LITE_CTRL_OP_S)


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

#define NTT_LITE_CTRL_RHS_CONST_EN_S       ((uint32_t) 27 )
#define NTT_LITE_CTRL_RHS_CONST_EN_M       ((uint32_t) 0x1)
#define NTT_LITE_CTRL_RHS_CONST_EN_V       (NTT_LITE_CTRL_RHS_CONST_EN_M << NTT_LITE_CTRL_RHS_CONST_EN_S)

#define NTT_LITE_CTRL_ROUND_DIS_S          ((uint32_t) 28 )
#define NTT_LITE_CTRL_ROUND_DIS_M          ((uint32_t) 0x1)
#define NTT_LITE_CTRL_ROUND_DIS_V          (NTT_LITE_CTRL_ROUND_DIS_M << NTT_LITE_CTRL_ROUND_DIS_S)

#define NTT_LITE_CTRL_BIGEND_EN_S          ((uint32_t) 29 )
#define NTT_LITE_CTRL_BIGEND_EN_M          ((uint32_t) 0x1)
#define NTT_LITE_CTRL_BIGEND_EN_V          (NTT_LITE_CTRL_BIGEND_EN_M << NTT_LITE_CTRL_BIGEND_EN_S)

#define NTT_LITE_STATUS_CHKNORM_S          ((uint32_t) 26 )
#define NTT_LITE_STATUS_CHKNORM_M          ((uint32_t) 0x1)
#define NTT_LITE_STATUS_CHKNORM_V          (NTT_LITE_STATUS_CHKNORM_M << NTT_LITE_STATUS_CHKNORM_S)

#define NTT_LITE_STATUS_REJSAMP_IP_S       ((uint32_t) 27 )
#define NTT_LITE_STATUS_REJSAMP_IP_M       ((uint32_t) 0x1)
#define NTT_LITE_STATUS_REJSAMP_IP_V       (NTT_LITE_STATUS_REJSAMP_IP_M << NTT_LITE_STATUS_REJSAMP_IP_S)

#define NTT_LITE_STATUS_BUSY_S             ((uint32_t) 30 )
#define NTT_LITE_STATUS_BUSY_M             ((uint32_t) 0x1)
#define NTT_LITE_STATUS_BUSY_V             (NTT_LITE_STATUS_BUSY_M << NTT_LITE_STATUS_BUSY_S)

#define NTT_LITE_STATUS_DONE_S             ((uint32_t) 31 )
#define NTT_LITE_STATUS_DONE_M             ((uint32_t) 0x1)
#define NTT_LITE_STATUS_DONE_V             (NTT_LITE_STATUS_DONE_M << NTT_LITE_STATUS_DONE_S)


#endif