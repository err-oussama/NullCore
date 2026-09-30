#ifndef CHECKSUM_H
#define CHECKSUM_H
#include <types.h>

uint32 checksum_word_sum(void *addr, uint16 len);
uint32 checksum_fold(uint32 sum);
uint8 checksum_is_valid(void *addr, uint16 len);
uint16 checksum_calc(void *addr, uint16 len);

#endif
