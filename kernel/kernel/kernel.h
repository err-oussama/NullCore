#ifndef KERNEL_H
#define KERNEL_H
#include "gdt.h"
#include "idt.h"
#include "kprint.h"
#include "multiboot_metadata.h"
#include "pic.h"
#include "pmm.h"
#include "vga_print.h"
#include "virtual_memory.h"

extern uint32 multiboot_header;
extern uint32 kernel_text;
extern uint32 kernel_rodata;
extern uint32 kernel_data;
extern uint32 kernel_bss;
extern uint32 kernel_end;

int is64bit();
void setup_hardware();

#endif
