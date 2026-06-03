#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <string.h>

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();
}
