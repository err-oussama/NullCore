#include "kprint.h"
#include <ata.h>
#include <kernel.h>
#include <kheap.h>
#include <kstring.h>
#include <multiboot_metadata.h>
#include <pci.h>
#include <pmio.h>
#include <pmm.h>
#include <rtl8139.h>
#include <types.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprint_wrn("========[ Working on: Refactoring ]========\n");
  pmm_info();

  show_multiboot_data(boot_info);
  show_physical_memory();
  /* show_bitmap(); */
}
