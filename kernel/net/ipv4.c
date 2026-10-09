#include <arp.h>
#include <byteorder.h>
#include <checksum.h>
#include <icmp.h>
#include <ipv4.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>
#include <tcp.h>
#include <types.h>
#include <udp.h>

static ipv4_iface_t ipv4_iface = {
    .dev_ip = {192, 168, 100, 2},
    .net_ip = {192, 168, 100, 0},
    .gateway = {192, 168, 100, 1},
    .mask = 24,
};

static int ipv4_next_id = 0;

uint16 ipv4_gen_id() { return ipv4_next_id++; }

const char *ipv4_proto(uint8 proto) {
  switch (proto) {
  case IPV4_PROTO_ICMP:
    return "ICMP";
  case IPV4_PROTO_TCP:
    return "TCP";
  case IPV4_PROTO_UDP:
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
  kprintf("TTL=%u proto=%s Checksum=0x%x\n", ttl, ipv4_proto(packet->protocol),
          checksum);

  kprintf("Src: %u.%u.%u.%u -> Dest: %u.%u.%u.%u\n", s_ip[0], s_ip[1], s_ip[2],
          s_ip[3], d_ip[0], d_ip[1], d_ip[2], d_ip[3]);
  kprintf("------------------------\n");
}

uint32 ipv4_is_fragment(ipv4_t *packet) {
  uint16 flags_frag = ntohs(packet->flags_frag);
  return (flags_frag & 0x2000) || (flags_frag & 0x1FFF);
}

void ipv4_init_packet(ipv4_t *packet, uint8 *dest_ip, uint8 proto,
                      void *payload, uint32 size) {
  packet->ver_ihl = 0x45;
  packet->tos = 0;
  packet->total_len = htons(size + 20);
  packet->id = htons(ipv4_gen_id());
  packet->flags_frag = 0;
  packet->ttl = 64;
  packet->protocol = proto;
  for (uint32 i = 0; i < 4; i++) {
    packet->dest_ip[i] = dest_ip[i];
    packet->src_ip[i] = ipv4_iface.dev_ip[i];
  }
  packet->checksum = 0;
  packet->checksum = checksum_calc(packet, sizeof(ipv4_t), 0);

  for (uint32 i = 0; i < size; i++) {
    packet->payload[i] = *(uint8 *)(payload + i);
  }
}

uint8 ipv4_same_net(uint8 *ip) {
  uint32 ip_host = ntohl(*(uint32 *)ip);
  uint32 dev_ip_host = htonl(*(uint32 *)ipv4_iface.dev_ip);
  uint32 bit_mask = 0xFFFFFFFF << (32 - ipv4_iface.mask);
  return (ip_host & bit_mask) == (dev_ip_host & bit_mask);
}

void ipv4_next_hop(uint8 *dest_ip, uint8 *next_hop_ip) {
  if (ipv4_same_net(dest_ip))
    for (uint32 i = 0; i < 4; i++)
      next_hop_ip[i] = dest_ip[i];
  else
    for (uint32 i = 0; i < 4; i++)
      next_hop_ip[i] = ipv4_iface.gateway[i];
}

uint8 *ipv4_dev_ip() { return ipv4_iface.dev_ip; }

void ipv4_send(uint8 *dest_ip, uint8 proto, void *payload, uint32 size) {

  ipv4_t *packet = pmm_alloc(1);
  if (!packet)
    return;
  ipv4_init_packet(packet, dest_ip, proto, payload, size);
  uint8 next_hop_ip[4];
  ipv4_next_hop(dest_ip, next_hop_ip);
  arp_send_ipv4_packet(next_hop_ip, packet, size + 20);
}

uint32 ipv4_pseudo_sum(uint8 *src_ip, uint8 *dest_ip, uint8 proto, uint16 len) {
  ipv4_pseudo_t pseudo;
  memcpy(dest_ip, pseudo.dest_ip, 4);
  memcpy(src_ip, pseudo.src_ip, 4);
  pseudo.protocol = proto;
  pseudo.len = htons(len);
  pseudo.zero = 0;
  return checksum_word_sum(&pseudo, sizeof(ipv4_pseudo_t));
}

void ipv4_handler(ipv4_t *packet) {
  if (!checksum_is_valid(packet, sizeof(ipv4_t), 0) || ipv4_is_fragment(packet))
    return;

  uint8 ihl = (packet->ver_ihl & 0x0F) * 4;
  if (ihl != 20)
    return;

  uint16 len = ntohs(packet->total_len) - ihl;
  uint32 pseudo_sum = 0;
  uint8 *src_ip = packet->src_ip;
  uint8 *dest_ip = packet->dest_ip;

  switch (packet->protocol) {
  case IPV4_PROTO_ICMP:
    icmp_handler(src_ip, (icmp_t *)packet->payload, len);
    break;
  case IPV4_PROTO_UDP:
    pseudo_sum = ipv4_pseudo_sum(src_ip, dest_ip, IPV4_PROTO_UDP, len);
    udp_handler(pseudo_sum, (udp_t *)packet->payload, len);
    break;
  case IPV4_PROTO_TCP:
    pseudo_sum = ipv4_pseudo_sum(src_ip, dest_ip, IPV4_PROTO_TCP, len);
    tcp_handler(pseudo_sum, src_ip, dest_ip, (tcp_t *)packet->payload, len);
    break;
  }
}
