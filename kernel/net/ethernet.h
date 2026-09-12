#ifndef ETHERNET_H
#define ETHERNET_H

#include <types.h>

#define ETHERNET_TYPE_IPV4 0x0800
#define ETHERNET_TYPE_ARP 0x0806

typedef struct __attribute__((packed)) {
  uint8 dest_MAC[6];
  uint8 src_MAC[6];
  uint16 type;
  uint8 payload[];
} ethernet_frame_t;

void ethernet_receive(uint8 *frame, uint16 len);
void ethernet_send(uint8 *dest_mac, uint16 ethertype, uint8 *payload,
                   uint16 payload_len);
void ethernet_poll();

#endif
