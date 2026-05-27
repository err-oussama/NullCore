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
  uint64 tick = pit_get_tick();
  tick = 0xFFFFFFFF;
  kprint_hex64(tick);
  uint32 i = 0;
  while (i++ < 0xFFFFFFFF)
    ;
  kprintf("\n");
  tick = pit_get_tick();
  kprint_hex64(tick);
}
