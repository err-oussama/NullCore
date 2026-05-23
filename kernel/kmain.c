#include "kernel.h"
#include "kprint.h"
#include "memory.h"
#include "multiboot_metadata.h"
#include "type.h"

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  // kprint("Memmory Management Unit: basic setup done.\n");
  // show_multiboot_data(boot_info);
  // show_physical_memory();
  // pmm_info();

  init_heap();

  void *addr = kmalloc(0x10);
  kprintf("Address 1: %p\n", addr);
  addr = kmalloc(0x4);
  kprintf("Address 2: %p\n", addr);
  addr = kmalloc(0x4);
  kprintf("Address 2: %p\n", addr);

  kprintf("--------------------\n");
  kmemory_dump_hex(get_heap_start(), 64);
}
