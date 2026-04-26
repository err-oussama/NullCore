#include "gdt.h"
#include "idt.h"
#include "kprint.h"

static gdt_entry gdt_entrys[8];

static gate_descriptor gate_descriptors[256];

void setup_GDT() {

  setup_gdt_entrys(gdt_entrys);

  gdt_register gdtr;
  gdtr.base = (uint32)&gdt_entrys;
  gdtr.limit = (sizeof(gdt_entry) * 8) - 1;
  lgdtr(&gdtr);
  kmemory_dump_hex(&gdt_entrys, sizeof(gdt_entry) * 8);
}

void kmain(void) {
  setup_GDT();

  while (1)
    ;
}
