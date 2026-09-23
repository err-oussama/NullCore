#include "types.h"
#include <byteorder.h>

uint16 swaps(uint16 x) {
  x &= 0xFFFF;
  return (x >> 8) | (x << 8);
}

uint16 htons(uint16 x) { return swaps(x); }

uint16 ntohs(uint16 x) { return swaps(x); }
