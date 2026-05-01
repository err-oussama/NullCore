#ifndef PMIO_H
#define PMIO_H

#include "type.h"

void outb(uint16 port, uint8 value);
void outw(uint16 port, uint16 value);
void outdw(uint16 port, uint32 value);

uint8 inb(uint16 port);
uint16 inw(uint16 port);
uint32 indw(uint16 port);

#endif
