#include "kernel.h"
#include "kprint.h"
#include "memory.h"
#include "multiboot_metadata.h"
#include "type.h"

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();

  void *addr = NULL;
  uint32 sizes[] = {0x10, 0x8, 0x5, 0x1, 0x3, 0xF};
  uint32 values[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  for (uint32 i = 0; i < 6; i++) {
    addr = kmalloc(sizes[i]);
    memset(addr, values[i], sizes[i]);
  }

  kmemory_dump_hex(get_heap_start(), 128);
}
