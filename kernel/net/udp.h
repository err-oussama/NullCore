#ifndef UDP_H
#define UDP_H

#include <ipv4.h>
#include <types.h>

#define UDP_MAX_PAYLOAD_SIZE 1450

// UDP: User Datagram Protocol

typedef struct __attribute__((packed)) {
  uint16 src_port;
  uint16 dest_port;
  uint16 len; // header + payload
  uint16 checksum;
  uint8 payload[];
} udp_t;

void udp_send(uint8 *dest_ip, uint16 src_port, uint16 dest_port, void *payload,
              uint16 len);

void udp_handler(uint32 pseudo_sum, udp_t *datagram, uint16 len);

#endif
