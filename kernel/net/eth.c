#include <arp.h>
#include <eth.h>
#include <ipv4.h>
#include <pmm.h>
#include <rtl8139.h>
#include <types.h>

#include <kprint.h>

static void *eth_tx_pool = NULL;
static uint8 eth_tx_index = 0;
static uint8 eth_tx_count = 10;

static void *eth_rx_queue = NULL;
static uint8 eth_rx_count = 10;
static uint8 eth_rx_index_w = 0; // write index
static uint8 eth_rx_index_r = 0; // read index

void eth_init() {
  pci_rtl8139_init();

  eth_tx_pool = pmm_alloc(4);
  if (!eth_tx_pool)
    kprintf("Ethernet transmit pool allocation failed\n");

  eth_rx_queue = pmm_alloc(4);
  if (!eth_rx_queue)
    kprintf("Ethernet receive queue allocation failed\n");
}

void eth_get_mac(void *mac_out) { pci_rtl8139_get_mac(mac_out); }
void eth_send(uint8 *dest_mac, uint16 type, void *payload, uint16 len) {
  if (!eth_tx_pool) {
    kprintf("TX buffer is NULL\n");
    return;
  }
  if (len > ETH_PAYLOAD_MAX_LEN) {
    kprintf("Payload length exceed maximum %u\n", ETH_PAYLOAD_MAX_LEN);
    return;
  }

  eth_frame_t *frame = eth_tx_pool + (eth_tx_index * ETH_FRAME_MAX_LEN);

  for (uint8 i = 0; i < 6; i++)
    frame->dest_mac[i] = dest_mac[i];

  frame->type = HTONS(type);

  eth_get_mac(&frame->src_mac);

  uint16 i = 0;
  while (i < len) {
    frame->payload[i] = *(uint8 *)(payload + i);
    i++;
  }
  while (i < ETH_PAYLOAD_MIN_LEN) {
    frame->payload[i] = 0;
    i++;
  }
  pci_rtl8139_transmit_packet(frame, i + sizeof(eth_frame_t));
  eth_tx_index = eth_tx_index + 1 == eth_tx_count ? 0 : eth_tx_index + 1;
}

void eth_receive(void *frame, uint16 len) {
  uint8 *slot = eth_rx_queue + (eth_rx_index_w * ETH_FRAME_MAX_LEN);
  *(uint16 *)slot = len;
  slot += sizeof(uint16);

  for (uint32 i = 0; i < len; i++)
    slot[i] = ((uint8 *)frame)[i];

  eth_rx_index_w++;
  if (eth_rx_index_w == eth_rx_count)
    eth_rx_index_w = 0;
}

void eth_poll() {
  while (eth_rx_index_r != eth_rx_index_w) {
    void *slot = eth_rx_queue + (eth_rx_index_r * ETH_FRAME_MAX_LEN);
    uint16 len = *(uint16 *)slot;
    eth_frame_t *frame = slot + 2;
    switch (frame->type) {
    case ETH_TYPE_ARP_NET:
      arp_handler((arp_t *)frame->payload);
      /* kprintf("ARP\n"); */
      break;
    case ETH_TYPE_IPV4_NET:
      ipv4_handler((ipv4_t *)frame->payload);
      /* kprintf("IPV4\n"); */
      break;
    case ETH_TYPE_IPV6_NET:
      kprintf("IPV6\n");
      break;
    default:
      kprintf("Unknown protocol %x\n\n", frame->type);
    }
    eth_rx_index_r = ++eth_rx_index_r == eth_rx_count ? 0 : eth_rx_index_r;
  }
  asm("hlt");
}
