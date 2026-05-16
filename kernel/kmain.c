#include "kernel.h"
#include "kprint.h"
#include "multiboot_metadata.h"
#include "pmm.h"

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  kprint("Memmory Management Unit: basic setup done.\n");

  show_multiboot_data(boot_info);
  show_physical_memory();
  pmm_info();
}
