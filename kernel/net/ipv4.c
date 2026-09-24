#include "types.h"
#include <byteorder.h>
#include <ipv4.h>
#include <kprint.h>

const char *ipv4_get_protocol(uint8 protocol) {
  switch (protocol) {
  case IPV4_PROTOCOL_ICMP:
    return "ICMP";
  case IPV4_PROTOCOL_TCP:
    return "TCP";
  case IPV4_PROTOCOL_UDP:
    return "UDP";
  }
  return "UNKNOWN";
}

void ipv4_dump(ipv4_t *packet) {
  uint16 flags_frag = ntohs(packet->flags_frag);
  uint8 ver = packet->ver_ihl >> 4;
  uint8 ihl = (packet->ver_ihl & 0x0F) * 4;
  uint8 tos = packet->tos;
  uint16 total_len = ntohs(packet->total_len);
  uint16 id = ntohs(packet->id);
  uint8 ttl = packet->ttl;
  uint16 checksum = ntohs(packet->checksum);
  uint8 *s_ip = packet->src_ip;
  uint8 *d_ip = packet->dest_ip;
  uint16 offset = (flags_frag & 0x1FFF) * 8;

  kprintf("------ IPv4 packet ------\n");
  kprintf("Ver=%u IHL=%u TOS=%u TotalLen=%u\n", ver, ihl, tos, total_len);
  kprintf("Id=%u Flags=[%s %s] Offset=%u\n", id,
          (flags_frag & 0x4000) ? "DF" : "--",
          (flags_frag & 0x2000) ? "MF" : "--", offset);
  kprintf("TTL=%u Protocol=%s Checksum=0x%x\n", ttl,
          ipv4_get_protocol(packet->protocol), checksum);

  kprintf("Src: %u.%u.%u.%u -> Dest: %u.%u.%u.%u\n", s_ip[0], s_ip[1], s_ip[2],
          s_ip[3], d_ip[0], d_ip[1], d_ip[2], d_ip[3]);
  kprintf("-------------------------\n");
}

uint32 ipv4_header_word_sum(void *header) {
  uint16 *words = header;
  uint32 sum = 0;
  for (uint32 i = 0; i < 10; i++)
    sum += words[i];
  return sum;
}

uint16 ipv4_checksum_is_valid(ipv4_t *packet) {
  uint32 sum = ipv4_header_word_sum(packet);
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);
  return sum == 0xFFFF;
}

uint32 ipv4_is_fragment(ipv4_t *packet) {
  uint16 flags_frag = ntohs(packet->flags_frag);
  return (flags_frag & 0x2000) || (flags_frag & 0x1FFF);
}

void ipv4_handler(ipv4_t *packet) {
  /* ipv4_dump(packet); */

  if (!ipv4_checksum_is_valid(packet) || ipv4_is_fragment(packet))
    return;

  switch (packet->protocol) {
  case IPV4_PROTOCOL_ICMP:
    kprintf("ICMP packet\n");
    break;
  case IPV4_PROTOCOL_TCP:
    kprintf("TCP packet\n");
    break;
  case IPV4_PROTOCOL_UDP:
    kprintf("UDP packet\n");
    break;
  }
}
