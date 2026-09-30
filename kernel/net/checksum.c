#include <checksum.h>

uint32 checksum_word_sum(void *addr, uint16 len) {
  uint16 *words = addr;
  uint32 sum = 0;
  for (uint16 i = 0; i < len / 2; i++)
    sum += words[i];
  if (len & 1)
    sum += ((uint8 *)addr)[len - 1];
  return sum;
}

uint32 checksum_fold(uint32 sum) {
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);
  return sum;
}

uint8 checksum_is_valid(void *addr, uint16 len) {
  return checksum_fold(checksum_word_sum(addr, len)) == 0xFFFF;
}

uint16 checksum_calc(void *addr, uint16 len) {
  return ~checksum_fold(checksum_word_sum(addr, len)) & 0xFFFF;
}
