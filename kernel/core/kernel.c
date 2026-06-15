#include "kernel.h"
#include "pit.h"

void setup_hardware() {
  setup_GDT();
  setup_IDT();
  pic_init();
  pit_init(1000);
  mmu_kernel_setup();
  activate_interrupt();
}
