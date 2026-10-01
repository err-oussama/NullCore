#ifndef UDP_H
#define UDP_H

#include <types.h>

// UDP: User Datagram Protocol

typedef struct __attribute__((packed)) {
  uint16 src_port;
  uint16 dest_port;
  uint16 len; // header + payload
  uint16 checksum;
  uint8 payload[];
} udp_t;

typedef struct __attribute__((packed)) {
  uint8 src_ip[4];
  uint8 dest_ip[4];
  uint8 zero;
  uint8 protocol;
  uint16 udp_len; // copy of udp_t.len
} udp_pseudo_t;

void udp_send(uint8 *dest_ip, uint16 src_port, uint16 dest_port, void *payload,
              uint16 len);

void udp_handler(uint8 *src_ip, uint8 *dest_ip, udp_t *datagram, uint16 len);

#endif
