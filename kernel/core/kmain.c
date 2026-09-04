#include "kprint.h"
#include <ata.h>
#include <kernel.h>
#include <kstring.h>
#include <pci.h>
#include <pmio.h>
#include <pmm.h>
#include <rtl8139.h>
#include <types.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprint_wrn("========[ Working on: Network (PCI) ]========\n");
  pci_setup();
  pci_rtl8139_init();

  ethernet_frame_t *packet = (void *)pmm_alloc();
  uint16 len = 64;
  uint8 dest_MAC[] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};

  for (int i = 0; i < 6; i++)
    packet->dest_MAC[i] = dest_MAC[i];

  for (int i = 0; i < 6; i++)
    packet->dest_MAC[i + 6] = dest_MAC[i];
  packet->type = 0x800;

  for (int i = 0; i < len - 14; i++) {
    packet->payload[i] = 0xAB;
  }

  for (uint8 i = 0; i < 20; i++) {
    pci_rtl8139_transmit_packet(packet, len);
  }
}
