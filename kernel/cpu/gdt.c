#include "gdt.h"

void gdt_set_entry(gdt_entry *entry, uint32 base, uint32 limit,
                   uint8 access_byte, uint8 flags) {

  entry->access_byte = access_byte;
  entry->base_high = base >> 24;
  entry->base_middle = (base >> 16);
  entry->base_low = base;
  entry->limit_low = limit;
  entry->granularity = (flags << 4) | ((limit >> 16) & 0b1111);
}

void gdt_set_TSS_64bit_descriptor(gdt_entry *entry, unsigned long base,
                                  uint32 limit, uint8 access_byte,
                                  uint8 flags) {

  gdt_set_entry(entry, base & 0x00000000ffffffff, limit, access_byte, flags);
  // unsigned long *extension = (unsigned long *)&entry[1];
  // *extension = base >> 32; // uncomment this in 64-bit
}

void setup_gdt_entrys(gdt_entry *entrys) {
  /*
        [
                  NULL,
                  Kernel code,
                  Kernel Data,
                  User 32-bit Code,
                  User Data,
                  User 64-bit Code,
                  TSS low 8 byte
                  TSS high 8 byte

        ]
  */
  // setup  NULL
  gdt_set_entry(&entrys[0], 0, 0, 0, 0);
  // setup kernel code descriptor
  gdt_set_entry(&entrys[1], 0, 0xfffff,
                GDT_ACC_ACCESSED | GDT_ACC_EXEC_READ | GDT_ACC_NON_CONFORMING |
                    GDT_ACC_TYPE_CODE_SEG | GDT_ACC_S_SEGMENT | GDT_ACC_DPL0 |
                    GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_32BIT | GDT_FLAG_OP_SIZE_32 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup kernel data descriptor
  gdt_set_entry(&entrys[2], 0, 0xfffff,
                GDT_ACC_ACCESSED | GDT_ACC_READ_WRITE | GDT_ACC_GROW_UP |
                    GDT_ACC_TYPE_DATA_SEG | GDT_ACC_S_SEGMENT | GDT_ACC_DPL0 |
                    GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_32BIT | GDT_FLAG_OP_SIZE_32 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup user code 32-bit mode descriptor
  gdt_set_entry(&entrys[3], 0, 0xfffff,
                GDT_ACC_ACCESSED | GDT_ACC_EXEC_READ | GDT_ACC_NON_CONFORMING |
                    GDT_ACC_TYPE_CODE_SEG | GDT_ACC_S_SEGMENT | GDT_ACC_DPL3 |
                    GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_32BIT | GDT_FLAG_OP_SIZE_32 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup user data descriptor
  gdt_set_entry(&entrys[4], 0, 0xfffff,
                GDT_ACC_ACCESSED | GDT_ACC_READ_WRITE | GDT_ACC_GROW_UP |
                    GDT_ACC_TYPE_DATA_SEG | GDT_ACC_S_SEGMENT | GDT_ACC_DPL3 |
                    GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_32BIT | GDT_FLAG_OP_SIZE_32 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup user code 64-bit mode descriptor
  gdt_set_entry(&entrys[5], 0, 0xfffff,
                GDT_ACC_ACCESSED | GDT_ACC_EXEC_READ | GDT_ACC_NON_CONFORMING |
                    GDT_ACC_TYPE_CODE_SEG | GDT_ACC_S_SEGMENT | GDT_ACC_DPL3 |
                    GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_64BIT | GDT_FLAG_OP_SIZE_16 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup TSS descriptor
  gdt_set_entry(&entrys[6], 0, 0x67, 0x89, 0x0);
}

void setup_GDT() {

  setup_gdt_entrys(gdt_entrys);

  gdt_register gdtr;
  gdtr.base = (uint32)&gdt_entrys;
  gdtr.limit = (sizeof(gdt_entry) * 8) - 1;
  lgdtr(&gdtr);
}
