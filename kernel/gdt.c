#include "gdt.h"

void gdt_set_entry(gdt_entry *entry, unsigned int base, unsigned int limit,
                   unsigned char access_byte, unsigned char flags) {

  entry->access_byte = access_byte;
  entry->base_high = base >> 24;
  entry->base_middle = (base >> 16);
  entry->base_low = base;
  entry->limit_low = limit;
  entry->granularity = (flags << 4) | ((limit >> 16) & 0b1111);
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
  gdt_set_entry(&entrys[1], 0, 0xffff,
                GDT_ACC_ACCESSED | GDT_ACC_EXEC_READ | GDT_ACC_CONFORMING |
                    GDT_ACC_TYPE_CODE_SEG | GDT_ACC_TYPE_CODE_DATA |
                    GDT_ACC_DPL0 | GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_64BIT | GDT_FLAG_OP_SIZE_16 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup kernel data descriptor
  gdt_set_entry(&entrys[2], 0, 0xffff,
                GDT_ACC_ACCESSED | GDT_ACC_READ_WRITE | GDT_ACC_GROW_DWON |
                    GDT_ACC_TYPE_DATA_SEG | GDT_ACC_TYPE_CODE_DATA |
                    GDT_ACC_DPL0 | GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_64BIT | GDT_FLAG_OP_SIZE_16 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup user code 32-bit mode descriptor
  gdt_set_entry(&entrys[3], 0, 0xffff,
                GDT_ACC_ACCESSED | GDT_ACC_EXEC_READ | GDT_ACC_CONFORMING |
                    GDT_ACC_TYPE_CODE_SEG | GDT_ACC_TYPE_CODE_DATA |
                    GDT_ACC_DPL3 | GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_32BIT | GDT_FLAG_OP_SIZE_32 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup user data descriptor
  gdt_set_entry(&entrys[4], 0, 0xffff,
                GDT_ACC_ACCESSED | GDT_ACC_READ_WRITE | GDT_ACC_GROW_DWON |
                    GDT_ACC_TYPE_DATA_SEG | GDT_ACC_TYPE_CODE_DATA |
                    GDT_ACC_DPL3 | GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_64BIT | GDT_FLAG_OP_SIZE_16 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup user code 64-bit mode descriptor
  gdt_set_entry(&entrys[5], 0, 0xffff,
                GDT_ACC_ACCESSED | GDT_ACC_EXEC_READ | GDT_ACC_CONFORMING |
                    GDT_ACC_TYPE_CODE_SEG | GDT_ACC_TYPE_CODE_DATA |
                    GDT_ACC_DPL3 | GDT_ACC_PRESENT,
                GDT_FLAG_AVL_0 | GDT_FLAG_64BIT | GDT_FLAG_OP_SIZE_16 |
                    GDT_FLAG_SEG_UNIT_4KB);
  // setup TSS descriptor
}
