#include "idt.h"
#include "isr.h"
#include "type.h"

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

  uint8 kernel_attr_gate =
      IDT_PRESENT | IDT_DPL0 | IDT_S_BIT0 | IDT_TYPE_32BIT_INTERRUPT_GATE;

  set_idt_entry(&gate_descriptors[0], (uint32)isr_divide_error_handler, 0x08,
                kernel_attr_gate);
  set_idt_entry(&gate_descriptors[0x20], (uint32)isr_timer_handler, 0x08,
                kernel_attr_gate);

  set_idt_entry(&gate_descriptors[0x21], (uint32)isr_keyboard_handler, 0x08,
                kernel_attr_gate);

  set_idt_entry(&gate_descriptors[0x0E], (uint32)isr_page_fault_handler, 0x08,
                kernel_attr_gate);

  set_idt_entry(&gate_descriptors[0x08], (uint32)isr_double_fault_handler, 0x08,
                kernel_attr_gate);

  set_idt_entry(&gate_descriptors[0x0D],
                (uint32)isr_general_protection_fault_handler, 0x08,
                kernel_attr_gate);

  set_idt_entry(&gate_descriptors[0x0A], (uint32)isr_invalid_TSS_handler, 0x08,
                kernel_attr_gate);

  set_idt_entry(&gate_descriptors[0x0B],
                (uint32)isr_segment_not_present_handler, 0x08,
                kernel_attr_gate);

  set_idt_entry(&gate_descriptors[0x0C],
                (uint32)isr_stack_segment_fault_handler, 0x08,
                kernel_attr_gate);

  set_idt_entry(&gate_descriptors[0x11], (uint32)isr_alignment_check_handler,
                0x08, kernel_attr_gate);

  lidtr(&idt_reg);
}
