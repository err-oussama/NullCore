#include "kernel.h"

void kmain(void) {
  gdt_entry gdt_entry[8];

  setup_gdt_entrys(gdt_entry);
  kmemory_dump_hex(&gdt_entry[0], sizeof(gdt_entry[0]) * 8);

  while (1)
    ;
}
