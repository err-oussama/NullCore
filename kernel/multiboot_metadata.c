#include "multiboot_metadata.h"

void show_multiboot_data(multiboot_info *boot_info) {
  kprint("\n----MULTIBOOT METADATA----\n");
  uint8 *ptr = (uint8 *)boot_info->mmap_addr;
  uint8 *end = ptr + boot_info->mmap_length;
  while (ptr < end) {

    mmap_entry *entry = (mmap_entry *)ptr;
    kprint("type: ");
    kprint_hex(entry->type);
    kprint(" | address: ");
    kprint_hex(entry->addr_low);
    kprint(" | len: ");
    kprint_hex(entry->len_low);

    kprint("\n");
    ptr += entry->size + 4;
  }
  kprint("-----------------------\n");
}

void init_pmp(multiboot_info *boot_info) {
  uint32 aligned_free_memory_start =
      (5 * 0x1000 + (uint32)(&kernel_end) + 0xFFF) & 0xFFFFF000;
  uint32 end_memory = 0x7FE0000; // this address excluded from free memory

  uint32 pool_size = end_memory - aligned_free_memory_start;
  pmm_init(aligned_free_memory_start, pool_size);
}
