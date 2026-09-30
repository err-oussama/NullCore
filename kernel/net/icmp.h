#ifndef ICMP_H
#define ICMP_H

#include <types.h>

// ICMP: Internet Control Message Protocol

#define ICMP_TYPE_ECHO_REPLY 0X0
#define ICMP_TYPE_DEST_UNREACHABLE 0X3
#define ICMP_TYPE_ECHO_REQUEST 0X8

typedef struct __attribute__((packed)) {
  uint8 type;
  uint8 code;
  uint16 checksum;
} icmp_t;

typedef struct __attribute__((packed)) {
  icmp_t icmp;
  uint16 id;
  uint16 seq;
  uint8 data[];
} icmp_echo_t;

typedef struct __attribute__((packed)) {
  icmp_t icmp;
  uint32 unused;
  uint8 data[];
} icmp_dest_unreachable_t;

typedef struct {
  uint16 id;
  uint16 seq;
  uint8 is_waiting;
} icmp_echo_session_t;

void icmp_handler(uint8 *ipv4, icmp_t *msg, uint16 len);
void icmp_echo_request(uint8 *ipv4, void *data, uint16 len);

#endif
