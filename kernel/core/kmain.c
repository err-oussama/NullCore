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
  for (int i = 0; i < 6; i++)
    packet->dest_MAC[i] = 0x81;
  for (int i = 6; i < 12; i++)
    packet->src_MAC[i] = 0x18;
  packet->type = 0x800;

  uint8 *payload = (void *)(((uint8 *)packet) + sizeof(ethernet_frame_t));
  for (int i = 0; i < len - 14; i++) {
    payload[i] = 0xAB;
  }

  pci_rtl8139_transmit_packet(packet, len, 0);
  pci_rtl8139_transmit_packet(packet, len, 1);
  pci_rtl8139_transmit_packet(packet, len, 2);
  pci_rtl8139_transmit_packet(packet, len, 3);
}
