#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();

  uint64 start = pit_get_tick();
  for (uint32 i = 0; i < 60; i++) {
    start = pit_get_tick();
    while (pit_get_tick() - start < 1000)
      ;
    kprintf("%d!! ", i + 1);
  }
}
