#include "types.h"
#include <byteorder.h>
#include <ipv4.h>
#include <kprint.h>

void ipv4_dump(ipv4_t *packet) {
  uint16 flags_frag = ntohs(packet->flags_frag);
  kprintf("Ver: %u\n", packet->ver_ihl >> 4);
  kprintf("IHL: %u\n", packet->ver_ihl & 0xF);
  kprintf("TOS: %u\n", packet->tos);
  kprintf("Total len: %u\n", ntohs(packet->total_len));
  kprintf("Id: %u\n", ntohs(packet->id));
  kprintf("DF: %u\n", (flags_frag >> 14) & 0x1);
  kprintf("MF: %u\n", (flags_frag >> 13) & 0x1);
  kprintf("Offset: %u\n", flags_frag & 0x1FFF);
  kprintf("TTL: %u\n", packet->ttl);
  kprintf("Protocol: %u\n", packet->protocol);
}

void ipv4_handler(ipv4_t *packet) { ipv4_dump(packet); }
