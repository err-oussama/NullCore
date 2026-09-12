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
  kprint_wrn("========[ Working on: Network ]========\n");
  pci_rtl8139_init();
  uint8 mac[6];
  pci_rtl8139_get_mac(mac);

  kprintf("Machine MAC address: [");
  for (uint8 i = 0; i < 6; i++)
    kprintf("%x%c", mac[i], i == 5 ? ']' : ':');
}
