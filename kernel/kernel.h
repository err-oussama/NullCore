#ifndef KERNEL_H
#define KERNEL_H
#include "gdt.h"
#include "kprint.h"
#include "vga_print.h"

typedef struct __attribute__((packed)) {
  uint32 flags; // bit N se = field N is valid

  // memory info -- valid if bit 0 set
  uint32 mem_lower; // KB of lower memory (below 1MB), typically 640KB
  uint32 mem_upper; // KB of upper memory (above 1MB)

  // boot device -- valid if bit 1 set
  uint32 boot_device; // which disk/partition was booted from

  // command line -- valid if bit 2 set
  uint32 cmdline; // physical address of command line string

  // modules -- valid if bit 3 set
  uint32 mods_count; // number of modules loaded
  uint32 mods_addr;  // address of first module structure

  // symbol table -- valid if bit 4 or 5 set
  uint32 syms[4]; // debug symbol info

  // memory map -- valid if bit 6 set ← most important
  uint32 mmap_length; // size of memory map in bytes
  uint32 mmap_addr;   // physical address of memory map entries

  // drivers -- valid if bit 7 set
  uint32 drivers_length; // size of drivers structure
  uint32 drivers_addr;   // address of drivers structure

  // BIOS config -- valid if bit 8 set
  uint32 config_table;

  // bootloader name -- valid if bit 9 set
  uint32 boot_loader_name; // address of bootloader name string;

  // (Advanced Power Management)
  // APM table -- valid if bit 10 set
  uint32 apm_table;

  // Video Electronics Standards Associations, Basic Input Ouput System
  // (VESA BIOS Extensions)
  // VBE info -- valid if bit 11 set
  uint32 vbe_control_info;
  uint32 vbe_mode_info;
  uint32 vbe_mode;
  uint32 vbe_interface_seg;
  uint32 vbe_interface_off;
  uint32 vbe_interface_len;

} multiboot_info;

int get_cs();
int is64bit();
#endif
