#include "kernel.h"

void gdt_set_entry(gdt_entry *entry, unsigned int base, unsigned int limit,
                   unsigned char access_byte, unsigned char flags) {

  entry->access_byte = access_byte;
  entry->base_high = base >> 24;
  entry->base_middle = (base >> 16);
  entry->base_low = base;
  entry->limit_low = limit;
  entry->granularity = (flags << 4) | ((limit >> 16) & 0b1111);
}
