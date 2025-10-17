#ifndef CW305_REGS_H_
#define CW305_REGS_H_

#include <stdint.h>

typedef volatile       uint8_t IO_U8;

typedef volatile const uint8_t O_U8;

typedef volatile       uint8_t I_U8;

typedef struct {
    I_U8  tx;
    O_U8  rx;
    IO_U8 status;
    IO_U8 trigger;
} cw305_regs_t;


#define CW305_RX_STATUS_S             ((uint32_t) 0  )
#define CW305_RX_STATUS_M             ((uint32_t) 0x1)
#define CW305_RX_STATUS_V             (CW305_RX_STATUS_M << CW305_RX_STATUS_S)

#define CW305_TX_STATUS_S             ((uint32_t) 1  )
#define CW305_TX_STATUS_M             ((uint32_t) 0x1)
#define CW305_TX_STATUS_V             (CW305_TX_STATUS_M << CW305_TX_STATUS_S)

#define CW305_TRIGGER_UP_S            ((uint32_t) 0  )
#define CW305_TRIGGER_UP_M            ((uint32_t) 0x1)
#define CW305_TRIGGER_UP_V            (CW305_TRIGGER_UP_M << CW305_TRIGGER_UP_S)


#define CW305_BASE_ADDR               ((uint32_t) 0x10041000)
#define CW305_REGS                    ((cw305_regs_t*) CW305_BASE_ADDR)

#endif