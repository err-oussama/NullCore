#include <byteorder.h>

uint16 htons(uint16 x) {
  x &= 0xFFFF;
  return (x << 8) | (x >> 8);
}
