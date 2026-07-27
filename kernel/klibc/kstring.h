#ifndef KSTRING_H
#define KSTRING_H
#include <types.h>

void memset(void *addr, uint32 c, uint32 size);
void memcpy(void *src, void *dest, uint32 size);
uint32 strlen(char *str);
void bezero(void *addr, uint32 size);

#endif
