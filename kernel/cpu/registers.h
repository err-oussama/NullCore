#ifndef REGISTERS_H
#define REGISTERS_H
#include <types.h>

void switch_esp(uint32 esp);
uint32 get_esp();

void disable_interrupt();
void enable_interrupt();

#endif
