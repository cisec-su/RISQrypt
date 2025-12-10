#include "cw305.h"
#include "cw305_regs.h"


void cw305_transmit_byte(const char data)
{
    while (((CW305_REGS->status) & CW305_STATUS_TX_V));
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
    while (!((CW305_REGS->status) & CW305_STATUS_RX_V));
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
    CW305_REGS->sca |= CW305_SCA_TRIGGER_UP_V;
}


void cw305_trigger_down() 
{
    CW305_REGS->sca &= ~CW305_SCA_TRIGGER_UP_V;
}


void cw305_done_set() 
{
    CW305_REGS->sca |= CW305_SCA_DONE_SET_V;
}


int cw305_is_done() 
{
    if (CW305_REGS->sca & CW305_SCA_DONE_SET_V) {
        return CW305_DONE_HIGH;
    }
    else {
        return CW305_DONE_LOW;
    }
}


void cw305_done_set_trigger_down() 
{
    CW305_REGS->sca = CW305_SCA_DONE_SET_V;
}