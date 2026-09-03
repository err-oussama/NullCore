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

  uint32 TCR = pci_rtl8139_indw(RTL8139_TCR_OFFSET);
  uint8 CMD = pci_rtl8139_inb(RTL8139_CMD_OFFSET);
  uint16 IMR = pci_rtl8139_inw(RTL8139_IMR_OFFSET);
  kprintf("TCR: 0x%x\n", TCR);
  kprintf("CMD: 0x%x\n", CMD);
  kprintf("IMR: 0x%x\n", IMR);

  pci_rtl8139_transmit_packet(packet, len, 0);
  uint16 TSD0 = pci_rtl8139_indw(RTL8139_TSD0_OFFSET);
  kprintf("TSD0: 0x%x\n", TSD0);
}
