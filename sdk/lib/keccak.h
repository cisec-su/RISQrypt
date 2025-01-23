#ifndef KECCAK_H
#define KECCAK_H


#include <stdint.h>



int keccak_init(uint32_t rate, uint32_t mask_en);


int keccak_absorb(uint32_t *share_0, uint32_t *share_1, unsigned int len);


int keccak_finish(uint32_t pad_word);


int keccak_squeeze(uint32_t *share_0, uint32_t *share_1, unsigned int len);


#endif