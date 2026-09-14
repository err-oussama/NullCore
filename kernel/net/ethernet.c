#include "types.h"
#include <ethernet.h>
#include <pmm.h>
#include <rtl8139.h>

#include <kprint.h>

void *eth_tx_pool = NULL;
uint8 eth_tx_index = 0;

uint8 eth_tx_count = 10;

void eth_init() {
  pci_rtl8139_init();
  eth_tx_pool = pmm_alloc(4);
  if (!eth_tx_pool) {
    kprintf("Ethernet transmit pool allocation faild\n");
    return;
  }
}
void eth_send(uint8 *dest_mac, uint16 ethertype, uint8 *payload,
              uint16 payload_len) {

  if (!eth_tx_pool) {
    kprintf("TX buffer is NULL\n");
    return;
  }
  if (payload_len > ETH_MTU) {
    kprintf("Payload length exceed maximum %u\n", ETH_MTU);
    return;
  }

  eth_frame_t *frame = eth_tx_pool + (eth_tx_index * ETH_FRAME_MAX_LEN);

  pci_rtl8139_get_mac(frame->src_mac);

  for (uint8 i = 0; i < 6; i++)
    frame->dest_mac[i] = dest_mac[i];

  frame->type = (ethertype & 0xFF) << 8 | (ethertype >> 8);

  uint16 i = 0;
  while (i < payload_len) {
    frame->payload[i] = payload[i];
    i++;
  }
  while (i < 60 - sizeof(eth_frame_t)) {
    frame->payload[i] = 0;
    i++;
  }
  pci_rtl8139_transmit_packet(frame, i + sizeof(eth_frame_t));
  eth_tx_index = eth_tx_index + 1 == eth_tx_count ? 0 : eth_tx_index + 1;
}
