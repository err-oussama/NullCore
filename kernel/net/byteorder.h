#ifndef BYTEORDER_H
#define BYTEORDER_H
#include <types.h>

#define HTONS(x) ((x >> 8) | ((x & 0xFF) << 8))

uint16 htons(uint16 x);
uint16 ntohs(uint16 x);
uint32 ntohl(uint32 x);
uint32 htonl(uint32 x);
#endif
