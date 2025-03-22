#ifndef KECCAK_H
#define KECCAK_H


#include <stdint.h>


#define KECCAK_MASK_EN  ((uint32_t) 0x1)
#define KECCAK_MASK_DIS ((uint32_t) 0x0)
#define KECCAK_NULL_PAD_WORD  ((uint32_t*) 0x0)


int keccak_init(uint32_t rate, uint32_t mask);


int keccak_absorb(const uint32_t *share_0, const uint32_t *share_1, unsigned int len);


int keccak_finish(const uint32_t *pad_word);


int keccak_squeeze(uint32_t *share_0, uint32_t *share_1, unsigned int len);


#endif