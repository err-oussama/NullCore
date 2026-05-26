#ifndef IDT_H
#define IDT_H
#include "isr.h"
#include "type.h"

// SELECTOR

#define RPL0 0x0
#define RPL1 0x1
#define RPL2 0x2
#define RPL3 0x3

#define IDT_TI_L 0x4
#define IDT_TI_G 0x0

// TYPE & ATTRIBUTE

#define IDT_S_BIT0 0x00

#define IDT_DPL0 0x00
#define IDT_DPL1 0x20
#define IDT_DPL2 0x40
#define IDT_DPL3 0x60

#define IDT_PRESENT 0x80
#define IDT_ABSENT 0x00

// IDT descriptor type
#define IDT_TYPE_RESERVED0 0x0
#define IDT_TYPE_RESERVED1 0x2
#define IDT_TYPE_RESERVED2 0x3
#define IDT_TYPE_RESERVED3 0x8
#define IDT_TYPE_RESERVED4 0xA
#define IDT_TYPE_RESERVED5 0xD

#define IDT_TYPE_16BIT_INTERRUPT_GATE 0x6
#define IDT_TYPE_32BIT_INTERRUPT_GATE 0xE
#define IDT_TYPE_TASK_GATE 0x5
#define IDT_TYPE_16BIT_TRAP_GATE 0x7
#define IDT_TYPE_32BIT_TRAP_GATE 0xF
// STRUCT

typedef struct __attribute__((packed)) {
  uint16 offset_low; // first 16 bit
  uint16 selector;
  uint8 reserved;
  uint8 type_attribute;
  uint16 offset_high;
} gate_descriptor;

typedef struct __attribute__((packed)) {
  uint16 limit;
  uint32 base;
} idtr;

static gate_descriptor gate_descriptors[256];

// FUNCTION

void set_idt_entry(gate_descriptor *gate, uint32 ISR_address, uint16 selector,
                   uint8 type_attr);

void lidtr(idtr *idt_r);
void sidtr(idtr *idt_r);
void activate_interrupt();
void setup_IDT();
#endif
