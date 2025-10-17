#include "cw305.h"
#include "cw305_regs.h"


void cw305_transmit_byte(const char data)
{
    while (((CW305_REGS->status) & CW305_TX_STATUS_V));
    for (volatile int i = 0; i < 100; i++);
    CW305_REGS->tx = data;
}


void cw305_transmit_string(char const *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        cw305_transmit_byte(*data++);
    }
}


void cw305_receive_byte(char *data)
{
    while (!((CW305_REGS->status) & CW305_RX_STATUS_V));
    *data = CW305_REGS->rx;
}


void cw305_receive_string(char *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        cw305_receive_byte(data++);
    }
}


void cw305_trigger_up() 
{
    CW305_REGS->trigger |= CW305_TRIGGER_UP_V;
}


void cw305_trigger_down() 
{
    CW305_REGS->trigger &= ~CW305_TRIGGER_UP_V;
}