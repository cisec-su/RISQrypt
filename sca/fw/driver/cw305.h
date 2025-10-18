#ifndef CW305_H_
#define CW305_H_

#include <stdlib.h>


#define CW305_DONE_HIGH   (0x1)
#define CW305_DONE_LOW    (0x0)


void cw305_transmit_byte(const char data);
void cw305_transmit_string(char const *data, size_t len);
void cw305_receive_byte(char *data);
void cw305_receive_string(char *data, size_t len);
void cw305_trigger_up();
void cw305_trigger_down();
void cw305_done_set();
int cw305_is_done();
void cw305_done_set_trigger_down();

#endif