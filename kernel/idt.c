#include "idt.h"

void set_idt_entry(gate_descriptor *gate, uint32 ISR_address, uint16 selector,
                   uint8 type_attr) {
  gate->selector = selector;
  gate->type_attribute = type_attr;
  gate->offset_low = ISR_address & 0xffff;
  gate->offset_high = ISR_address >> 0x10;
  gate->reserved = 0;
}

void setup_dummy_isr() {

  for (int i = 0; i < 256; i++)
    set_idt_entry(&gate_descriptors[i], (uint32)isr_dummy, 1 << 3,
                  IDT_PRESENT | IDT_DPL0 | IDT_S_BIT0 |
                      IDT_TYPE_32BIT_INTERRUPT_GATE);
}

void setup_IDT() {
  setup_dummy_isr();
  idtr idt_reg;
  idt_reg.base = (uint32)gate_descriptors;
  idt_reg.limit = sizeof(gate_descriptor) * 256 - 1;
  set_idt_entry(&gate_descriptors[0], (uint32)isr_divide_error_handler, 0x08,
                IDT_PRESENT | IDT_DPL0 | IDT_S_BIT0 |
                    IDT_TYPE_32BIT_INTERRUPT_GATE);

  set_idt_entry(&gate_descriptors[0x20], (uint32)isr_timer_handler, 0x08,
                IDT_PRESENT | IDT_DPL0 | IDT_S_BIT0 |
                    IDT_TYPE_32BIT_INTERRUPT_GATE);

  set_idt_entry(&gate_descriptors[0x21], (uint32)isr_keyboard_handler, 0x08,
                IDT_PRESENT | IDT_DPL0 | IDT_S_BIT0 |
                    IDT_TYPE_32BIT_INTERRUPT_GATE);
  lidtr(&idt_reg);
}
