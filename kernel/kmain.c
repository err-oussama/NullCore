#include "gdt.h"
#include "idt.h"
#include "kprint.h"
#include "multiboot_metadata.h"
#include "pic.h"
#include "pmm.h"

void setup_hardware() {

  setup_GDT();
  setup_IDT();
  pic_init();
  activate_interrupt();
}

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  kprint("\nPhyscial memory pool start: 0x");
  kprint_hex(pmm_get_pool_start());
  kprint("\nPhyscial memory pool size:  0x");
  kprint_hex(pmm_get_pool_size());
  kprint("\n");
  kprint_dec((pmm_get_pool_size() / 0x1000) / 0x8);

  setup_hardware();
}
