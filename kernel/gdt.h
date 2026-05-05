#ifndef GDT_H
#define GDT_H
#include "type.h"

// FLAG BYTE
#define GDT_FLAG_AVL_1 0b1
#define GDT_FLAG_AVL_0 0b0

#define GDT_FLAG_64BIT 0b10
#define GDT_FLAG_32BIT 0b00

#define GDT_FLAG_OP_SIZE_16 0b000
#define GDT_FLAG_OP_SIZE_32 0b0100

#define GDT_FLAG_SEG_UNIT_1B 0b0000
#define GDT_FLAG_SEG_UNIT_4KB 0b1000

// ACCESS BYTE

#define GDT_ACC_ACCESSED 0b1
#define GDT_ACC_NOT_ACCESSED 0b0

#define GDT_ACC_EXEC_ONLY 0b00
#define GDT_ACC_EXEC_READ 0b10

#define GDT_ACC_READ_ONLY 0b00
#define GDT_ACC_READ_WRITE 0b10

#define GDT_ACC_NON_CONFORMING 0b000
#define GDT_ACC_CONFORMING 0b100

#define GDT_ACC_GROW_UP 0b000
#define GDT_ACC_GROW_DWON 0b100

#define GDT_ACC_TYPE_DATA_SEG 0b0000
#define GDT_ACC_TYPE_CODE_SEG 0b1000

#define GDT_ACC_S_SYSTEM 0b00000
#define GDT_ACC_S_SEGMENT 0b10000

#define GDT_ACC_DPL0 0b0000000
#define GDT_ACC_DPL1 0b0100000
#define GDT_ACC_DPL2 0b1000000
#define GDT_ACC_DPL3 0b1100000

#define GDT_ACC_PRESENT 0b10000000
#define GDT_ACC_ABSENT 0b00000000

#define GDT_TYPE_32BIT_TSS_AVAILABLE 0x9
#define GDT_TYPE_32BIT_TSS_BUSY 0xB
#define GDT_TYPE_32BIT_CALL_GATE 0xC
#define GDT_TYPE_16BIT_CALL_GATE 0x4
#define GDT_TYPE_16BIT_TASK_GATE 0x1

typedef struct __attribute__((packed)) {
  uint16 limit_low;
  uint16 base_low;
  uint8 base_middle;
  uint8 access_byte;
  uint8 granularity; // limit high and flags
  uint8 base_high;
} gdt_entry;

typedef struct __attribute__((packed)) {
  uint16 limit; // size of the gdt_entry vector by bytes
  uint32 base;  // pointer to whre the gdt_entry vector start;
} gdt_register;

void lgdtr(gdt_register *gdt_ptr);
void sgdtr(gdt_register *gdt_ptr);

void gdt_set_entry(gdt_entry *entry, uint32 base, uint32 limit,
                   uint8 access_byte, uint8 flags);
void gdt_set_TSS_descriptor(gdt_entry *entry, unsigned long base, uint32 limit,
                            uint8 access_byte, uint8 flags);

void setup_gdt_entrys(gdt_entry *entrys);

static gdt_entry gdt_entrys[8];
void setup_GDT();
#endif
