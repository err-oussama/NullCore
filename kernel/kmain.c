#include "kernel.h"
#include "kprint.h"
void kmain(void) {
  gdt_entry null_entry;
  gdt_set_entry(&null_entry, 0, 0b11111111111111111111, 0, 0b1111);
  kmemory_dump_bin(&null_entry, sizeof(null_entry));

  while (1)
    ;
}
