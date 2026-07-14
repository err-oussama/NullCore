#ifndef KERNEL_H
#define KERNEL_H
#include <multiboot_metadata.h>
#include <vmm.h>

extern uint32 multiboot_header;
extern uint32 kernel_text;
extern uint32 kernel_rodata;
extern uint32 kernel_data;
extern uint32 kernel_bss;
extern uint32 kernel_end;

void init_kernel(multiboot_info *boot_info);

#endif
