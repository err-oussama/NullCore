#ifndef ETHERNET_H
#define ETHERNET_H

#include <types.h>

#define ETH_TYPE_IPV4 0x0800
#define ETH_TYPE_ARP 0x0806

#define ETH_FRAME_MAX_LEN 1518 // full frame: 14 header + 1500 payload + 4 CRC
#define ETH_MTU 1500           // (Maximum Transmission Unit) Max payload size

typedef struct __attribute__((packed)) {
  uint8 dest_mac[6];
  uint8 src_mac[6];
  uint16 type;
  uint8 payload[];
} eth_frame_t;

void eth_receive(uint8 *frame, uint16 len);
void eth_send(uint8 *dest_mac, uint16 ethertype, uint8 *payload,
              uint16 payload_len);
void eth_poll();

#endif
