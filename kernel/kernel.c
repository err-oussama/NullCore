#include "kernel.h"

void setup_hardware() {

  setup_GDT();
  setup_IDT();
  pic_init();
  activate_interrupt();
  mmu_kernel_setup();
}
