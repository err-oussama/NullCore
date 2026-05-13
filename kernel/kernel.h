#ifndef KERNEL_H
#define KERNEL_H
#include "gdt.h"
#include "idt.h"
#include "kprint.h"
#include "multiboot_metadata.h"
#include "pic.h"
#include "pmm.h"
#include "vga_print.h"

extern uint32 kernel_end;

int get_cs();
int is64bit();
void setup_hardware();

#endif
