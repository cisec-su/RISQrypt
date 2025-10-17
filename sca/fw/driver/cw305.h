#ifndef CW305_H_
#define CW305_H_

#include <stdlib.h>


void cw305_transmit_byte(const char data);
void cw305_transmit_string(char const *data, size_t len);
void cw305_receive_byte(char *data);
void cw305_receive_string(char *data, size_t len);
void cw305_trigger_up();
void cw305_trigger_down();

#endif