#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "kernel.h"
#include "kprint.h"
#include "pic.h"
#include "type.h"

extern uint32 kernel_end;

void kmain(multiboot_info *boot_info) {
  uint32 free_start_aligned =
      (5 * 0x1000 + (uint32)(&kernel_end) + 0xFFF) & 0xFFFFF000;
  kclear_screen() uint8 *ptr = (uint8 *)boot_info->mmap_addr;
  uint8 *end = ptr + boot_info->mmap_length;
  kprint_hex((uint32)&kernel_end);
  kprint("\n");
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
  setup_GDT();
  setup_IDT();
  pic_init();
  activate_interrupt();
}
