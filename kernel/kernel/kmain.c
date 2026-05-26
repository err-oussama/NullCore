#include "kernel.h"
#include "kprint.h"
#include "memory.h"
#include "multiboot_metadata.h"
#include "type.h"

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();
  uint64 a = 0xFFFFFFFFFFF;
  uint64 b = 0xFFFFFFFEFFF;

  uint64 c = a - b;
  kmemory_dump_bin(&c, sizeof(uint64));
}
