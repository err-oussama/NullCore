#include "sys/types.h"
#include "types.h"
#include <kstring.h>

void memset(void *addr, uint32 c, uint32 size) {
  uint8 *p = (uint8 *)addr;
  while (size--) {
    p[size] = c;
  }
}

void bezero(void *addr, uint32 size) { memset(addr, 0, size); }

void memcpy(void *src, void *dest, uint32 size) {
  uint8 *s = src;
  uint8 *d = dest;

  uint32 i;

  if (!size)
    return;

  if (src > dest) {
    i = -1;
    while (++i < size)
      d[i] = s[i];

  } else {
    i = size;
    while (i--)
      d[i] = s[i];
  }
}

uint32 strlen(char *str) {
  uint32 len = 0;
  while (str[len])
    len++;
  return len;
}
