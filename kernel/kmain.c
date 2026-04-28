#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "kprint.h"
#include "type.h"
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

void setup_dummy_isr() {

  for (int i = 0; i < 256; i++)
    set_idt_entry(&gate_descriptors[i], (uint32)isr_dummy, 1 << 3,
                  IDT_PRESENT | IDT_DPL0 | IDT_S_BIT0 |
                      IDT_TYPE_32BIT_INTERRUPT_GATE);
}

void kmain(void) {
  setup_GDT();

  setup_dummy_isr();
  idtr idt_reg;
  idt_reg.base = (uint32)gate_descriptors;
  idt_reg.limit = sizeof(gate_descriptor) * 256 - 1;
  set_idt_entry(&gate_descriptors[0], (uint32)isr_dived_error_handler, 1 << 3,
                IDT_PRESENT | IDT_DPL0 | IDT_S_BIT0 |
                    IDT_TYPE_32BIT_INTERRUPT_GATE);
  lidtr(&idt_reg);
  activate_interrupt();
  int a = 0;
  int b = 10 / a;
  while (1)
    ;
}
