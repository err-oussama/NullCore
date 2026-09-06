#include "multiboot_metadata.h"
#include "types.h"
#include <pmm.h>

char *type_meaning(uint8 type) {
  if (type == 1)
    return "Available       ";
  if (type == 2)
    return "Reserved        ";
  if (type == 3)
    return "ACPI Reclaimable";
  if (type == 4)
    return "ACPI NVS        ";
  if (type == 5)
    return "Bad RAM         ";
  return "Unknown         ";
}

void show_multiboot_data(multiboot_info *boot_info) {
  kprint_str("\n------------MULTIBOOT METADATA------------\n");
  uint8 *ptr = (uint8 *)boot_info->mmap_addr;
  uint8 *end = ptr + boot_info->mmap_length;

  mmap_entry *entry = (mmap_entry *)ptr;
  while (ptr < end) {

    entry = (mmap_entry *)ptr;
    kprintf("type: 0x%x %s", entry->type, type_meaning(entry->type));
    kprintf(", address: 0x%x", entry->addr_low);
    kprintf(", size: 0x%x\n", entry->len_low);
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

  pmm_init((void *)aligned_frame, end_pmp - aligned_frame);
}
