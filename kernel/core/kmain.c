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
  kprintf("#########\n");
  uint32 size = 40;
  void *add = pmm_alloc(size);
  /* pmm_free(add, size); */
  pmm_info();
}
