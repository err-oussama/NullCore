#include "kprint.h"
#include <ata.h>
#include <kernel.h>
#include <kstring.h>
#include <pmm.h>
#include <types.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprint_wrn("========[ Working on: Network (PCI) ]========\n");
}
