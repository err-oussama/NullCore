#ifndef IPV4_H
#define IPV4_H

#include <types.h>

#define IPV4_PROTOCOL_ICMP 0x1 // Internet Control Message Protocol
#define IPV4_PROTOCOL_UDP 0x11 // User Datagram Protocol
#define TPV4_PROTOCOL_TCP 0x6  // Transmission Control Protocol

// IP options are not supported;
// header length (IHL) is expected to always be 5 (20-byte header, no options)
typedef struct __attribute__((packed)) {

  // version (4 bits) + header length (4 bits) in uints of 32-bit words
  uint8 ver_ihl;
  uint8 tos; // type of service

  uint16 total_len;

  uint16 id; // identification

  uint16 flags_frag; // flags (3 bits) + fragment offset (13 bits)

  uint8 ttl;      // time to live
  uint8 protocol; // TCP, UDP, ...

  uint16 checksum; // header checksum

  uint8 src_ip[4];

  uint8 dest_ip[4];

  uint8 data[];
} ipv4_t;

void ipv4_handler(ipv4_t *packet);
#endif
