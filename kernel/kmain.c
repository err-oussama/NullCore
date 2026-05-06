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
  kprint("Physcial memory pool start: 0x");
  kprint_hex(pmm_get_pool_start());
  kprint("\nPhyscial memory pool size:  0x");
  kprint_hex(pmm_get_pool_size());

  setup_hardware();
}
