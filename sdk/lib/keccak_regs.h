#ifndef KECCAK_REGS_H
#define KECCAK_REGS_H


#include <stdint.h>


typedef volatile uint32_t IO;

typedef volatile uint32_t I;

typedef const volatile uint32_t O;


#define SHARES (2)


typedef struct
{
    IO ctrl;                     // BASE_ADDR + 0x0000
    IO data_len;                 // BASE_ADDR + 0x0004
    I seed_ptr;                 // BASE_ADDR + 0x0008
    I din_addr[SHARES];         // BASE_ADDR + 0x000C to BASE_ADDR + (0x000C + (SHARES-1)*4)
    O rfu_0x20[8 - SHARES];      // BASE_ADDR + 0x000C to BASE_ADDR + (0x000C + (SHARES-1)*4)
    IO dout_addr[SHARES];        // BASE_ADDR + 0x002C to BASE_ADDR + (0x002C + (SHARES-1)*4)
} keccak_regs_t;


#define KECCAK_CTRL_CMD_S         ((uint32_t) 0)
#define KECCAK_CTRL_CMD_M         ((uint32_t) 0xF)
#define KECCAK_CTRL_CMD_V         (KECCAK_CTRL_CMD_M << KECCAK_CTRL_CMD_S)
#define KECCAK_CTRL_CMD_IDLE      (((uint32_t) 0x0) << KECCAK_CTRL_CMD_S)
#define KECCAK_CTRL_CMD_ABSORB    (((uint32_t) 0x1) << KECCAK_CTRL_CMD_S)
#define KECCAK_CTRL_CMD_SQUEEZE   (((uint32_t) 0x2) << KECCAK_CTRL_CMD_S)
#define KECCAK_CTRL_CMD_PAD       (((uint32_t) 0x3) << KECCAK_CTRL_CMD_S)
#define KECCAK_CTRL_CMD_RST       (((uint32_t) 0xF) << KECCAK_CTRL_CMD_S)

#define KECCAK_CTRL_RATE_S        ((uint32_t) 4)
#define KECCAK_CTRL_RATE_M        ((uint32_t) 0x1F)
#define KECCAK_CTRL_RATE_V        (KECCAK_CTRL_RATE_M << KECCAK_CTRL_RATE_S)

#define KECCAK_CTRL_S_DIS_S       ((uint32_t) 9)
#define KECCAK_CTRL_S_DIS_M       ((uint32_t)((1U << SHARES) - 1))
#define KECCAK_CTRL_S_DIS_V       (KECCAK_CTRL_S_DIS_M << KECCAK_CTRL_S_DIS_S)
#define KECCAK_CTRL_S_DIS_1S      ((KECCAK_CTRL_S_DIS_M - 1) << KECCAK_CTRL_S_DIS_S)

#define KECCAK_CTRL_PERM_DIS_S    ((uint32_t) 14)
#define KECCAK_CTRL_PERM_DIS_M    ((uint32_t) 0x1)
#define KECCAK_CTRL_PERM_DIS_V    (KECCAK_CTRL_PERM_DIS_M << KECCAK_CTRL_PERM_DIS_S)

#define KECCAK_CTRL_BUSY_S        ((uint32_t) 30)
#define KECCAK_CTRL_BUSY_M        ((uint32_t) 0x1)
#define KECCAK_CTRL_BUSY_V        (KECCAK_CTRL_BUSY_M << KECCAK_CTRL_BUSY_S)

#define KECCAK_CTRL_DONE_S        ((uint32_t) 31)
#define KECCAK_CTRL_DONE_M        ((uint32_t) 0x1)
#define KECCAK_CTRL_DONE_V        (KECCAK_CTRL_DONE_M << KECCAK_CTRL_DONE_S)


#endif // KECCAK_REGS_H