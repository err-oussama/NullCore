#include "kernel.h"
#include "kprint.h"
#include "type.h"

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  kprint("Memmory Management Unit: basic setup done.\n");
}
