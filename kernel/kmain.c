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

  void *addr = kmalloc(0xF0);
  addr = kmalloc(0xF0);
  kprintf("this %p, %s, %d, [%c] [%d]", 0xBBBBBBBB, "this is god", 100, 'h',
          200);
}
