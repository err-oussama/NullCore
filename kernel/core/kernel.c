#include "kernel.h"
#include <gdt.h>
#include <idt.h>
#include <kheap.h>
#include <pic.h>
#include <pit.h>
#include <registers.h>

#include <task.h>
void setup_hardware() {
  setup_GDT();
  setup_IDT();
  pic_init();
  pit_init(1000);
  mmu_kernel_setup();
}

void init_kernel(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();
  task_init();
  /* enable_interrupt(); */
}
