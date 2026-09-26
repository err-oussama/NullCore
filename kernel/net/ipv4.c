#include "types.h"
#include <byteorder.h>
#include <ipv4.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>

static ipv4_iface_t ipv4_iface;
static int ipv4_next_id = 0;

uint16 ipv4_get_id() { return ipv4_next_id++; }

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

void ipv4_init_iface() {

  ipv4_iface.dev_ip[0] = 192;
  ipv4_iface.dev_ip[1] = 168;
  ipv4_iface.dev_ip[2] = 100;
  ipv4_iface.dev_ip[3] = 2;

  ipv4_iface.net_ip[0] = 192;
  ipv4_iface.net_ip[1] = 168;
  ipv4_iface.net_ip[2] = 100;
  ipv4_iface.net_ip[3] = 0;

  ipv4_iface.gateway[0] = 0;
  ipv4_iface.gateway[1] = 0;
  ipv4_iface.gateway[2] = 0;
  ipv4_iface.gateway[3] = 0;

  ipv4_iface.mask = 24;
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

uint16 ipv4_calc_checksum(ipv4_t *packet) {
  uint32 sum = ipv4_header_word_sum(packet);
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);
  return ~sum & 0xFFFF;
}

void ipv4_init_packet(ipv4_t *packet, uint8 *dest_ip, uint8 protocol,
                      void *payload, uint32 size) {
  packet->ver_ihl = 0x45;
  packet->tos = 0;
  packet->total_len = htons(size + 20);
  packet->id = htons(ipv4_get_id());
  packet->flags_frag = 0;
  packet->ttl = 64;
  packet->protocol = protocol;
  for (uint32 i = 0; i < 4; i++) {
    packet->dest_ip[i] = dest_ip[i];
    packet->src_ip[i] = ipv4_iface.dev_ip[i];
  }
  packet->checksum = 0;
  packet->checksum = ipv4_calc_checksum(packet);

  for (uint32 i = 0; i < size; i++) {
    packet->payload[i] = *(uint8 *)(payload + i);
  }
}

uint8 ipv4_is_in_same_net(uint8 *ip) {
  uint32 ip_host = ntohl(*(uint32 *)ip);
  uint32 dev_ip_host = htonl(*(uint32 *)ipv4_iface.dev_ip);
  uint32 bit_mask = 0xFFFFFFFF << (32 - ipv4_iface.mask);
  kprintf("%x <> %x\n", (ip_host & bit_mask), (dev_ip_host & bit_mask));
  return (ip_host & bit_mask) == (dev_ip_host & bit_mask);
}

void ipv4_send(uint8 *dest_ip, uint8 protocol, void *payload, uint32 size) {
  ipv4_t *packet = pmm_alloc(1);
  if (!packet)
    return;
  ipv4_init_packet(packet, dest_ip, protocol, payload, size);
  kprintf("Is ip in same net: %u\n", ipv4_is_in_same_net(dest_ip));

  // packet = ipv4_build_packet(dest, payload)
  //
  // next_hop_ip = none
  // if dest_ip in local_netowrk:
  // 		next_hop_ip = dest_ip
  // else:
  //		next_hop_ip = default_getway
  //
  // next_hop_mac = get_mac_of_ip(next_hop_ip)
  // if next_hop_mac:
  // 		eth_send(next_hop_mac, packet)
  // else
  //		arp_request(next_hop_ip)
  //		arp_enqueue(next_hop_ip, packet)
}
