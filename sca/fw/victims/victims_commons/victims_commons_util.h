#ifndef VICTIMS_COMMONS_H
#define VICTIMS_COMMONS_H

#include <stdint.h>


void vcu_ntt_lite_reset_state(void);

void vcu_sleep(uint32_t bound);

uint8_t vcu_prng_on(uint8_t* p, uint8_t len);

uint8_t vcu_prng_off(uint8_t* p, uint8_t len);

void zero_stack(uint32_t size);

#endif // VICTIMS_COMMONS_H