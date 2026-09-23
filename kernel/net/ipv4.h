#ifndef IPV4_H
#define IPV4_H

#include <types.h>

#define IPV4_PROTOCOL_ICMP 0x1 // Internet Control Message Protocol
#define IPV4_PROTOCOL_UDP 0x11 // User Datagram Protocol
#define IPV4_PROTOCOL_TCP 0x6  // Transmission Control Protocol

// IP options are not supported;
// header length (IHL) is expected to always be 5 (20-byte header, no options)
typedef struct __attribute__((packed)) {

  // 0-3: (4 bits) IP version
  // 4-7: (4 bits) header length in units of 32-bit words
  uint8 ver_ihl;

  // type of service: priority/Qos hints for how routers should treat this
  // packet (e.g. low-delay vs high-throughput); mostly unused today, set to 0
  uint8 tos;

  // length of this IP packet/fragment: header + payload
  uint16 total_len;

  // identifies the IP packet;
  // same value on all fragments of one packet, used to reassemble them.
  uint16 id;

  // bits 13-15: flags
  // 	bit 15: reserved, always 0
  // 	bit 14: DF (Don't Fragment)
  // 	bit 13: MF (More Fragments)
  // bits 0-12: fragment offset (in units of 8 bytes)
  uint16 flags_frag; // flags (3 bits) + fragment offset (13 bits)

  // hop limit: decremented by 1 at each hop; packet is dropped once it hits 0
  // (prevents packets looping forever)
  uint8 ttl; // time to live

  // upper-layer protocol carried in the payload (TCP, UDP, ICMP, ...)
  uint8 protocol;

  // checksum of the IP header only (not the payload);
  uint16 checksum;

  uint8 src_ip[4];

  uint8 dest_ip[4];

  uint8 payload[];
} ipv4_t;

void ipv4_handler(ipv4_t *packet);
#endif
