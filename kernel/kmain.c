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
  show_physical_memory();
}
