#include "kernel.h"
#include "kprint.h"
#include "type.h"
#include "virtual_memory.h"

void kmain(multiboot_info *boot_info) {
  kclear_screen();

  init_pmp(boot_info);

  setup_hardware();
  mmu_kernel_memory_map_pages();
  kprint("virtual memory setup\n");

  uint32 *p = (uint32 *)0xFFFFFFF;
  uint32 v = *p++;
}
