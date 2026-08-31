#include "pci.h"
#include "types.h"
#include <kprint.h>
#include <pmio.h>
#include <rtl8139.h>

#include <pmm.h>

pci_device_t *rtl8139 = NULL;

uint8 *rx_buffer = NULL;

uint8 *pci_rtl8139_get_rx_buffer() { return rx_buffer; }

void pci_rtl8139_init() {
  void *RX_first_4KB = (void *)pmm_alloc();
  void *RX_second_4KB = (void *)pmm_alloc();
  void *RX_third_4KB = (void *)pmm_alloc();

  if (RX_first_4KB + 0x1000 != RX_second_4KB ||
      RX_second_4KB + 0x1000 != RX_third_4KB) {
    kprintf("RTL_8139 Error: faild to allocate RX ring buffer\n");
    return;
  }
  rx_buffer = RX_first_4KB;
  rtl8139 = pci_find_device(0x10EC, 0x8139);
  if (!rtl8139) {
    kprintf("RTL8139 Error: Faild to find the device\n");
    return;
  }
  pci_rtl8139_outb(RTL8139_CMD_OFFSET, RTL8139_CMD_RST);
  while (pci_rtl8139_inb(RTL8139_CMD_OFFSET) & RTL8139_CMD_RST)
    ;
  // pci_rtl8139_outb(RTL8139_CMD_OFFSET, RTL8139_CMD_TE | RTL8139_CMD_RE);
  // pci_rtl8139_outdw(RTL8139_RBSTART_OFFSET, (uint32)RX_first_4KB);
  pci_rtl8139_outb(RTL8139_CMD_OFFSET, RTL8139_CMD_TE);
  kprintf("RTL8139 init done\n");
}

uint8 pci_rtl8139_inb(uint32 regis) {
  return inb(rtl8139->bars[0].address + regis);
}
uint16 pci_rtl8139_inw(uint32 regis) {
  return inw(rtl8139->bars[0].address + regis);
}
uint32 pci_rtl8139_indw(uint32 regis) {
  return indw(rtl8139->bars[0].address + regis);
}
void pci_rtl8139_outb(uint32 regis, uint8 value) {
  outb(rtl8139->bars[0].address + regis, value);
}
void pci_rtl8139_outw(uint32 regis, uint16 value) {
  outw(rtl8139->bars[0].address + regis, value);
}
void pci_rtl8139_outdw(uint32 regis, uint32 value) {
  outdw(rtl8139->bars[0].address + regis, value);
}

void pci_rtl8139_transmit_packet(ethernet_frame_t *packet, uint16 len,
                                 uint32 TSD_N) {

  if (TSD_N > 3) {
    kprintf("[PCI_RTL8139 Error]: TSD_N must be 0 <= TSD_N <= 3, not %i",
            TSD_N);
    return;
  }
  uint32 TSAD_offset = RTL8139_TSAD0_OFFSET + (TSD_N * 0x4);
  uint32 TSD_offset = RTL8139_TSD0_OFFSET + (TSD_N * 0x4);
  pci_rtl8139_outdw(TSAD_offset, (uint32)packet);
  pci_rtl8139_outdw(TSD_offset, len);
}
