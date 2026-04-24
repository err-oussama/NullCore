#include "idt.h"

void set_idt_entry(gate_descriptor *gate, uint32 ISR_address, uint16 selector,
                   uint8 type_attribute) {
  gate->selector = selector;
  gate->type_attribute = type_attribute;
  gate->offset_low = ISR_address & 0xffff;
  gate->offset_high = ISR_address >> 0x10;
  gate->reserved = 0;
}
