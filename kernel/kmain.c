#include "kernel.h"

void kmain(multiboot_info *boot_info) {
  kclear_screen();

  init_pmp(boot_info);

  for (uint32 i = 0; i < 100; i++) {
    kprint_dec(pmm_is_frame_free(i));
    kprint(" ");
  }

  setup_hardware();
}
