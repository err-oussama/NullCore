#include "multiboot_metadata.h"
#include "type.h"
#include "vga_print.h"

void show_multiboot_data(multiboot_info *boot_info) {
  kprint_str("\n------------MULTIBOOT METADATA------------\n");
  uint8 *ptr = (uint8 *)boot_info->mmap_addr;
  uint8 *end = ptr + boot_info->mmap_length;

  mmap_entry *entry = (mmap_entry *)ptr;
  while (ptr < end) {

    entry = (mmap_entry *)ptr;
    kprint_str("type: ");
    kprint_hex(entry->type);
    kprint_str(", address: 0x");
    kprint_hex(entry->addr_low);
    kprint_str(", size: 0x");
    kprint_hex(entry->len_low);

    kprint_str("\n");
    ptr += entry->size + 4;
  }
  kprint_str("------------------------------------------\n");
}

void init_pmp(multiboot_info *boot_info) {
  uint32 aligned_frame = ((uint32)(&kernel_end) + 0xFFF) & ~0xFFF;
  uint32 end_pmp = 0;
  uint8 *ptr = (uint8 *)boot_info->mmap_addr;
  uint8 *end = ptr + boot_info->mmap_length;

  mmap_entry *entry = (mmap_entry *)ptr;

  uint32 i = 0;
  while (ptr < end) {

    entry = (mmap_entry *)ptr;
    if (entry->type == 1) {
      i++;
    }
    if (i == 2) {
      end_pmp = entry->addr_low + entry->len_low;
      break;
    }
    ptr += entry->size + 4;
  }

  pmm_init(aligned_frame, end_pmp - aligned_frame);
}
