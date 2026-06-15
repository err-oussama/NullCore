#include <string.h>
#include <type.h>

void memset(void *addr, uint32 c, uint32 size) {
  uint8 *p = (uint8 *)addr;
  while (size--) {
    p[size] = c;
  }
}

void memcpy(void *src, void *dest, uint32 size) {
  uint8 *s = src;
  uint8 *d = dest;

  uint32 i;

  if (!size)
    return;

  if (src > dest) {
    i = -1;
    while (++i < size)
      s[i] = d[i];

  } else {
    i = size;
    while (i--)
      s[i] = d[i];
  }
}
