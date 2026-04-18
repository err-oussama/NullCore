#ifndef GDT_H
#define GDT_H

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

#define GDT_ACC_TYPE_KERN_DATA 0b00000
#define GDT_ACC_TYPE_CODE_DATA 0b10000

#define GDT_ACC_DPL0 0b0000000
#define GDT_ACC_DPL1 0b0100000
#define GDT_ACC_DPL2 0b1000000
#define GDT_ACC_DPL3 0b1100000

#define GDT_ACC_PRESENT 0b10000000
#define GDT_ACC_ABSENT 0b00000000

typedef struct __attribute__((packed)) {
  unsigned short limit_low;
  unsigned short base_low;
  unsigned char base_middle;
  unsigned char access_byte;
  unsigned char granularity; // limit high and flags
  unsigned char base_high;
} gdt_entry;

typedef struct __attribute__((packed)) {
  unsigned short limit; // size of the gdt_entry vector by bytes
  unsigned int base;    // pointer to whre the gdt_entry vector start;
} gdt_register;

void lgdtr(gdt_register *gdt_ptr);
void sgdtr(gdt_register *gdt_ptr);

void gdt_set_entry(gdt_entry *entry, unsigned int base, unsigned int limit,
                   unsigned char access_byte, unsigned char flags);

void setup_gdt_entrys(gdt_entry *entrys);

#endif
