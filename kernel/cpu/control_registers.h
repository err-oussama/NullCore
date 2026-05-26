#ifndef CONTROL_REGISTERS_H
#define CONTROL_REGISTERS_H

#include <type.h>

// Control Registers

void write_cr0(uint32 cr0_v);
uint32 read_cr0();

void write_cr3(uint32 cr3_v);
uint32 read_cr3();

#endif
