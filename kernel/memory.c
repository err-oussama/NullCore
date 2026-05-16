
#include "memory.h"
#include "kernel.h"
#include "multiboot_metadata.h"

void memset(void *addr, uint32 c, uint32 size) {
  uint8 *p = (uint8 *)addr;
  while (size--) {
    p[size] = c;
  }
}

void show_physical_memory() {

  kprint("\n------------------KERNEL------------------");
  kprint("\nmultiboot header: 0x");
  kprint_hex((uint32)&multiboot_header);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_text - (uint32)&multiboot_header);

  kprint("\n   .text        : 0x");
  kprint_hex((uint32)&kernel_text);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_rodata - (uint32)&kernel_text);

  kprint("\n   .rodata      : 0x");
  kprint_hex((uint32)&kernel_rodata);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_data - (uint32)&kernel_rodata);

  kprint("\n   .data        : 0x");
  kprint_hex((uint32)&kernel_data);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_bss - (uint32)&kernel_data);

  kprint("\n   .bss         : 0x");
  kprint_hex((uint32)&kernel_bss);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_end - (uint32)&kernel_bss);

  kprint("\n   end of kernel: 0x");
  kprint_hex((uint32)&kernel_end);
  kprint("\n------------------------------------------\n");
}
