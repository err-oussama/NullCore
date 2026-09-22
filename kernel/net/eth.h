#ifndef ETH_H
#define ETH_H

#include <types.h>

#include <byteorder.h>

#define ETH_TYPE_IPV6 0x86DD
#define ETH_TYPE_IPV6_HOST ETH_TYPE_IPV6
#define ETH_TYPE_IPV6_NET HTONS(ETH_TYPE_IPV6_HOST)

#define ETH_TYPE_IPV4 0x0800
#define ETH_TYPE_IPV4_HOST ETH_TYPE_IPV4
#define ETH_TYPE_IPV4_NET HTONS(ETH_TYPE_IPV4_HOST)

#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_ARP_HOST ETH_TYPE_ARP
#define ETH_TYPE_ARP_NET HTONS(ETH_TYPE_ARP_HOST)

#define ETH_FRAME_MIN_LEN 60   // full frame: 14 header + 46 payload
#define ETH_FRAME_MAX_LEN 1514 // full frame: 14 header + 1500 payload
//                                (CRC added by hardware, not stored here)
#define ETH_MTU 1500 // (Maximum Transmission Unit) Max payload size

#define ETH_PAYLOAD_MAX_LEN ETH_MTU
#define ETH_PAYLOAD_MIN_LEN 46

#define ETH_CRC_SIZE 4 // Cyclic Redundancy Check

typedef struct __attribute__((packed)) {
  uint8 dest_mac[6];
  uint8 src_mac[6];
  uint16 type;
  uint8 payload[];
} eth_frame_t;

void eth_init();
void eth_get_mac(void *mac_out);

void eth_receive(void *frame, uint16 len);
void eth_send(uint8 *dest_mac, uint16 type, void *payload, uint16 len);

void eth_poll();

#endif
