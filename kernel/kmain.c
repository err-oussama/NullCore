#include "gdt.h"
#include "idt.h"
#include "kernel.h"
#include "kprint.h"
#include "vga_print.h"
void kmain(void) {
  gdt_entry gdt_entrys[8];

  setup_gdt_entrys(gdt_entrys);

  int cs = get_cs() >> 3;
  gdt_register gdtr = {0};

  kprint_bin(IDT_DPL1 | IDT_PRESENT | IDT_S_BIT1);
  kprint("\n");
  sgdtr(&gdtr);
  kprint_dec(gdtr.base);
  kprint("\n");
  kprint_dec(sizeof(void *));
  kprint("\n");
  kprint("\n");
  kprint_dec(gdtr.limit);
  kprint("\n");
  gdt_entry *entrys = (gdt_entry *)gdtr.base;
  vga_memory_dump_bin(&entrys[1], sizeof(gdt_entry));

  kprint_dec(cs);
  while (1)
    ;
}
