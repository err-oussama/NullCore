#include "pci.h"
#include "types.h"
#include <kprint.h>
#include <pmio.h>
#include <rtl8139.h>

#include <pmm.h>
#define RX_BUFFER_SIZE 8192

pci_device_t *rtl8139 = NULL;

uint8 *rx_buffer = NULL;
uint32 offset = 0;

uint8 tsd_n = 0;

uint8 *pci_rtl8139_get_rx_buffer() { return rx_buffer; }

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

void pci_rtl8139_init() {
  void *RX_first_4KB = (void *)pmm_alloc();
  void *RX_second_4KB = (void *)pmm_alloc();
  void *RX_third_4KB = (void *)pmm_alloc();

  if (RX_first_4KB + 0x1000 != RX_second_4KB ||
      RX_second_4KB + 0x1000 != RX_third_4KB) {
    kprint_err("RTL_8139 Error: faild to allocate RX ring buffer\n");
    return;
  }
  rx_buffer = RX_first_4KB;
  rtl8139 = pci_find_device(0x10EC, 0x8139);
  if (!rtl8139) {
    kprint_err("RTL8139 Error: Faild to find the device\n");
    return;
  }
  pci_rtl8139_outb(RTL8139_CMD_OFFSET, RTL8139_CMD_RST);
  while (pci_rtl8139_inb(RTL8139_CMD_OFFSET) & RTL8139_CMD_RST)
    ;

  uint32 RBSTART = (uint32)rx_buffer;
  uint32 RCR = RTL8139_RCR_AAP;
  uint16 IMR = RTL8139_IMR_ROK;
  uint32 TCR = RTL8139_TCR_LBK_LOOPBACK;
  uint8 CMD = RTL8139_CMD_TE | RTL8139_CMD_RE;

  pci_rtl8139_outdw(RTL8139_RBSTART_OFFSET, RBSTART);
  pci_rtl8139_outdw(RTL8139_RCR_OFFSET, RCR);
  pci_rtl8139_outb(RTL8139_CMD_OFFSET, CMD);
  pci_rtl8139_outdw(RTL8139_TCR_OFFSET, TCR);
  pci_rtl8139_outw(RTL8139_IMR_OFFSET, IMR);

  uint32 TCR_after = pci_rtl8139_indw(RTL8139_TCR_OFFSET);
  if (!(TCR_after & TCR)) {
    kprint_err("Faild init TCR: ");
    kprintf("0x%x\n", TCR_after & (~0x7FC00000));
    return;
  }

  uint16 IMR_after = pci_rtl8139_inw(RTL8139_IMR_OFFSET);
  if (!(IMR_after & IMR)) {
    kprint_err("Faild to init IMR\n");
    return;
  }
  uint8 CMD_after = pci_rtl8139_inb(RTL8139_CMD_OFFSET);
  if (!(CMD_after & CMD)) {
    kprint_err("Faild to init IMR\n");
    return;
  }

  kprintf("RTL8139 init success\n");
}

void pci_rtl8139_transmit_packet(ethernet_frame_t *packet, uint16 len) {

  uint32 TSAD_offset = RTL8139_TSAD0_OFFSET + (tsd_n * 0x4);
  uint32 TSD_offset = RTL8139_TSD0_OFFSET + (tsd_n * 0x4);
  tsd_n = tsd_n == 3 ? 0 : tsd_n + 1;
  pci_rtl8139_outdw(TSAD_offset, (uint32)packet);
  pci_rtl8139_outdw(TSD_offset, len);
}

void pci_rtl8139_receive_packet() {
  uint16 status = pci_rtl8139_inw(RTL8139_ISR_OFFSET);
  if (status & RTL8139_ISR_ROK) {
    while (!(pci_rtl8139_inb(RTL8139_CMD_OFFSET) & RTL8139_CMD_BUFE)) {
      kprintf("CBR: 0x%x, CAPR: 0x%x\n", pci_rtl8139_inw(RTL8139_CBR_OFFSET),
              pci_rtl8139_inw(RTL8139_CBR_OFFSET));
      rtl8139_rx_header_t *packet_header = (void *)&rx_buffer[offset];
      offset += (packet_header->length + 4 + 3) & ~3;
      pci_rtl8139_outw(RTL8139_CAPR_OFFSET, offset - 16);
    }
  }
  pci_rtl8139_outw(RTL8139_ISR_OFFSET, 0x1);
}
