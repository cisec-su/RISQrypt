#ifndef X2X_REGS_H
#define X2X_REGS_H


#include <stdint.h>


typedef volatile uint32_t IO;

typedef volatile uint32_t I;

typedef const volatile uint32_t O;


#define SHARES (2)


typedef struct
{
    IO ctrl;                     // BASE_ADDR + 0x0000
    IO data_len;                 // BASE_ADDR + 0x0004
    I seed[2];                   // BASE_ADDR + 0x0008
    IO modulus;                  // BASE_ADDR + 0x0004    
    I din_addr[SHARES];          // BASE_ADDR + 0x000C to BASE_ADDR + (0x000C + (SHARES-1)*4)
    O rfu_0x20[8 - SHARES];      // BASE_ADDR + 0x000C to BASE_ADDR + (0x000C + (SHARES-1)*4)
    IO dout_addr[SHARES];        // BASE_ADDR + 0x002C to BASE_ADDR + (0x002C + (SHARES-1)*4)
} x2x_regs_t;


#define X2X_CTRL_START_S          ((uint32_t) 0)
#define X2X_CTRL_START_M          ((uint32_t) 0x1)
#define X2X_CTRL_START_V          (X2X_CTRL_START_M << X2X_CTRL_START_S)

#define X2X_CTRL_RESET_S          ((uint32_t) 1)
#define X2X_CTRL_RESET_M          ((uint32_t) 0x1)
#define X2X_CTRL_RESET_V          (X2X_CTRL_RESET_M << X2X_CTRL_RESET_S)

#define X2X_CTRL_CONV_MODE_S      ((uint32_t) 2)
#define X2X_CTRL_CONV_MODE_M      ((uint32_t) 0x1)
#define X2X_CTRL_CONV_MODE_V      (X2X_CTRL_CONV_MODE_M << X2X_CTRL_CONV_MODE_S)
#define X2X_CTRL_CONV_MODE_A2B    (((uint32_t) 0x0) << X2X_CTRL_CONV_MODE_S)
#define X2X_CTRL_CONV_MODE_B2A    (((uint32_t) 0x1) << X2X_CTRL_CONV_MODE_S)

#define X2X_CTRL_DATA_TYPE_S      ((uint32_t) 3)
#define X2X_CTRL_DATA_TYPE_M      ((uint32_t) 0x1)
#define X2X_CTRL_DATA_TYPE_V      (X2X_CTRL_DATA_TYPE_M << X2X_CTRL_DATA_TYPE_S)
#define X2X_CTRL_DATA_TYPE_POW2   (((uint32_t) 0x0) << X2X_CTRL_DATA_TYPE_S)
#define X2X_CTRL_DATA_TYPE_PRIME  (((uint32_t) 0x1) << X2X_CTRL_DATA_TYPE_S)

#define X2X_CTRL_DUAL_MODE_EN_S   ((uint32_t) 4)
#define X2X_CTRL_DUAL_MODE_EN_M   ((uint32_t) 0x1)
#define X2X_CTRL_DUAL_MODE_EN_V   (X2X_CTRL_DUAL_MODE_EN_M << X2X_CTRL_DUAL_MODE_EN_S)

#define X2X_CTRL_SRC_SHARE_S      ((uint32_t) 5)
#define X2X_CTRL_SRC_SHARE_M      ((uint32_t) 0x1)
#define X2X_CTRL_SRC_SHARE_V      (X2X_CTRL_SRC_SHARE_M << X2X_CTRL_SRC_SHARE_S)

#define X2X_CTRL_LOG_MODULUS_S    ((uint32_t) 6)
#define X2X_CTRL_LOG_MODULUS_M    ((uint32_t) 0x1F)
#define X2X_CTRL_LOG_MODULUS_V    (X2X_CTRL_LOG_MODULUS_M << X2X_CTRL_LOG_MODULUS_S)

#define X2X_CTRL_B2A_BIT_EN_S     ((uint32_t) 11)
#define X2X_CTRL_B2A_BIT_EN_M     ((uint32_t) 0x1)
#define X2X_CTRL_B2A_BIT_EN_V     (X2X_CTRL_B2A_BIT_EN_M << X2X_CTRL_B2A_BIT_EN_S)

#define X2X_CTRL_REJS_EN_S        ((uint32_t) 12)
#define X2X_CTRL_REJS_EN_M        ((uint32_t) 0x1)
#define X2X_CTRL_REJS_EN_V        (X2X_CTRL_REJS_EN_M << X2X_CTRL_REJS_EN_S)

#define X2X_CTRL_LOG_STRIDE_S     ((uint32_t) 13)
#define X2X_CTRL_LOG_STRIDE_M     ((uint32_t) 0x7)
#define X2X_CTRL_LOG_STRIDE_V     (X2X_CTRL_LOG_STRIDE_M << X2X_CTRL_LOG_STRIDE_S)

#define X2X_CTRL_SEED_BUSY_S      ((uint32_t) 29)
#define X2X_CTRL_SEED_BUSY_M      ((uint32_t) 0x1)
#define X2X_CTRL_SEED_BUSY_V      (X2X_CTRL_SEED_BUSY_M << X2X_CTRL_SEED_BUSY_S)

#define X2X_CTRL_BUSY_S           ((uint32_t) 30)
#define X2X_CTRL_BUSY_M           ((uint32_t) 0x1)
#define X2X_CTRL_BUSY_V           (X2X_CTRL_BUSY_M << X2X_CTRL_BUSY_S)

#define X2X_CTRL_DONE_S           ((uint32_t) 31)
#define X2X_CTRL_DONE_M           ((uint32_t) 0x1)
#define X2X_CTRL_DONE_V           (X2X_CTRL_DONE_M << X2X_CTRL_DONE_S)


#endif // X2X_REGS_H