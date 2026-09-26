#include "types.h"
#include <byteorder.h>

uint16 swaps(uint16 x) {
  x &= 0xFFFF;
  return (x >> 8) | (x << 8);
}

uint32 swapl(uint32 x) {
  uint8 *byte = (uint8 *)&x;
  uint8 rev[4];

  rev[3] = byte[0];
  rev[2] = byte[1];
  rev[1] = byte[2];
  rev[0] = byte[3];

  return *(uint32 *)rev;
}

uint16 htons(uint16 x) { return swaps(x); }

uint16 ntohs(uint16 x) { return swaps(x); }

uint32 htonl(uint32 x) { return swapl(x); }

uint32 ntohl(uint32 x) { return swapl(x); }
