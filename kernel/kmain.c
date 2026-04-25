#include "gdt.h"
#include "idt.h"
#include "kernel.h"
#include "kprint.h"
#include "vga_print.h"
void kmain(void) {
  gdt_entry gdt_entrys[8];

  setup_gdt_entrys(gdt_entrys);

  gdt_register gdtr;
  gdtr.base = (uint32)&gdt_entrys;
  gdtr.limit = sizeof(gdt_entry) * 8;

  lgdtr(&gdtr);
  gdt_entry *entrys = (gdt_entry *)gdtr.base;
  kmemory_dump_hex(&gdt_entrys, sizeof(gdt_entry) * 8);

  while (1)
    ;
}
