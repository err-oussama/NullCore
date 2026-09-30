#ifndef UDP_H
#define UDP_H

#include <types.h>

typedef struct __attribute__((packed)) {
  uint16 src_port;
  uint16 dest_port;
  uint16 length;
  uint16 checksum;
  uint8 payload[];
} udp_t;

typedef struct __attribute__((packed)) {
  uint8 src_ip[4];
  uint8 dest_ip[4];
  uint8 zero;
  uint8 protocol;
  uint16 udp_len;
} udp_pseudo_t;

#endif
