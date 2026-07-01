#ifndef CONTROL_REGISTERS_H
#define CONTROL_REGISTERS_H

#include <type.h>

// Control Registers

void write_cr0(uint32 cr0_v); // store flags
uint32 read_cr0();

void write_cr3(uint32 cr3_v); // CR3 store the page directoy pointer
uint32 read_cr3();

uint32 read_cr2(); // store the cause page fault
#endif
