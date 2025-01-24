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
    IO stride;     // BASE_ADDR + 0x0000000C
} ntt_lite_regs_t;



#define NTT_LITE_CTRL_CMD_S                ((uint32_t) 0  )
#define NTT_LITE_CTRL_CMD_M                ((uint32_t) 0xF)
#define NTT_LITE_CTRL_CMD_V                (NTT_LITE_CTRL_CMD_M << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_IDLE             (((uint32_t) 0x0) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_RST              (((uint32_t) 0x1) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_LOAD_Q           (((uint32_t) 0x2) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_LOAD_TWIDDLE     (((uint32_t) 0x3) << NTT_LITE_CTRL_CMD_S)
#define NTT_LITE_CTRL_CMD_LOAD_POLY        (((uint32_t) 0x4) << NTT_LITE_CTRL_CMD_S)

#define NTT_LITE_CTRL_LOGN_S               ((uint32_t) 4  )
#define NTT_LITE_CTRL_LOGN_M               ((uint32_t) 0xF)
#define NTT_LITE_CTRL_LOGN_V               (NTT_LITE_CTRL_LOGN_M << NTT_LITE_CTRL_LOGN_S)

#define NTT_LITE_CTRL_MODE_S               ((uint32_t) 8  )
#define NTT_LITE_CTRL_MODE_M               ((uint32_t) 0x7)
#define NTT_LITE_CTRL_MODE_V               (NTT_LITE_CTRL_MODE_M << NTT_LITE_CTRL_MODE_S)
#define NTT_LITE_CTRL_MODE_NTT             (((uint32_t) 0x0) << NTT_LITE_CTRL_MODE_S)
#define NTT_LITE_CTRL_MODE_INTT            (((uint32_t) 0x1) << NTT_LITE_CTRL_MODE_S)
#define NTT_LITE_CTRL_MODE_PWM             (((uint32_t) 0x2) << NTT_LITE_CTRL_MODE_S)

#define NTT_LITE_CTRL_OTF_S                ((uint32_t) 11 )
#define NTT_LITE_CTRL_OTF_M                ((uint32_t) 0x1)
#define NTT_LITE_CTRL_OTF_V                (NTT_LITE_CTRL_OTF_M << NTT_LITE_CTRL_OTF_S)
#define NTT_LITE_CTRL_OTF_DIS              (((uint32_t) 0x0) << NTT_LITE_CTRL_OTF_S)
#define NTT_LITE_CTRL_OTF_EN               (((uint32_t) 0x1) << NTT_LITE_CTRL_OTF_S)

#define NTT_LITE_CTRL_LOGQ_S               ((uint32_t) 12 )
#define NTT_LITE_CTRL_LOGQ_M               ((uint32_t) 0x3)
#define NTT_LITE_CTRL_LOGQ_V               (NTT_LITE_CTRL_LOGQ_M << NTT_LITE_CTRL_LOGQ_S)
#define NTT_LITE_CTRL_LOGQ_32              (((uint32_t) 0x0) << NTT_LITE_CTRL_LOGQ_S)
#define NTT_LITE_CTRL_LOGQ_64              (((uint32_t) 0x1) << NTT_LITE_CTRL_LOGQ_S)

#define NTT_LITE_CTRL_BUSY_S               ((uint32_t) 30 )
#define NTT_LITE_CTRL_BUSY_M               ((uint32_t) 0x1)
#define NTT_LITE_CTRL_BUSY_V               (NTT_LITE_CTRL_BUSY_M << NTT_LITE_CTRL_BUSY_S)

#define NTT_LITE_CTRL_DONE_S               ((uint32_t) 31 )
#define NTT_LITE_CTRL_DONE_M               ((uint32_t) 0x1)
#define NTT_LITE_CTRL_DONE_V               (NTT_LITE_CTRL_DONE_M << NTT_LITE_CTRL_DONE_S)


#endif