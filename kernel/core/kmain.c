#include "kprint.h"
#include <ata.h>
#include <kernel.h>
#include <kstring.h>
#include <pmm.h>
#include <types.h>

#include <pci.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprint_wrn("========[ Working on: Network (PCI) ]========\n");
  pci_setup();
  pci_print_device(-1);
  pci_device_t *rtl8139 = pci_find_device(0x10EC, 0x8139);
  if (!rtl8139)
    return;

  for (int i = 0; i < 6; i++) {
    kprintf("BAR%i: %i, 0x%x, size: 0x%x\n", i, rtl8139->bars[i].is_io_space,
            rtl8139->bars[i].address, rtl8139->bars[i].size);
  }
  uint8 port = rtl8139->bars[0].address;
  kprintf("0x%x\n", port);
}
