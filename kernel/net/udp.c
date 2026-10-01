#include <byteorder.h>
#include <checksum.h>
#include <ipv4.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>
#include <types.h>
#include <udp.h>

void udp_dump(udp_t *datagram) {
  kprintf("----- UDP -----\n");
  kprintf("src: %u, dest: %u, len: %u\n", ntohs(datagram->src_port),
          ntohs(datagram->dest_port), ntohs(datagram->len));
  kprintf("---------------\n");
}

uint32 udp_pseudo_sum(uint8 *src_ip, uint8 *dest_ip, uint16 len) {
  udp_pseudo_t pseudo;
  memcpy(dest_ip, pseudo.dest_ip, 4);
  memcpy(src_ip, pseudo.src_ip, 4);
  pseudo.zero = 0;
  pseudo.udp_len = htons(len);
  pseudo.protocol = IPV4_PROTOCOL_UDP;
  return checksum_word_sum(&pseudo, sizeof(udp_pseudo_t));
}

uint8 udp_is_valid(uint8 *src_ip, uint8 *dest_ip, udp_t *datagram, uint16 len) {
  uint32 sum = checksum_word_sum(datagram, len);
  sum += udp_pseudo_sum(src_ip, dest_ip, len);
  return checksum_fold(sum) == 0xFFFF;
}

void udp_handler(uint8 *src_ip, uint8 *dest_ip, udp_t *datagram, uint16 len) {
  if (!udp_is_valid(src_ip, dest_ip, datagram, len))
    return;
  udp_dump(datagram);
  for (uint32 i = 0; i < len - (sizeof(udp_t)); i++)
    kprintf("%c", datagram->payload[i]);
  kprintf("");
}

void udp_send(uint8 *dest_ip, uint16 src_port, uint16 dest_port, void *payload,
              uint16 len) {
  uint16 total_len = len + sizeof(udp_t);
  if (total_len > 1450)
    return;

  udp_t *datagram = pmm_alloc(1);
  if (!datagram)
    return;

  datagram->src_port = htons(src_port);
  datagram->dest_port = htons(dest_port);
  datagram->len = htons(total_len);
  datagram->checksum = 0;
  for (uint32 i = 0; i < len; i++)
    datagram->payload[i] = *(uint8 *)(payload + i);

  uint32 sum = checksum_word_sum(datagram, total_len);
  sum += udp_pseudo_sum(ipv4_get_dev_ip(), dest_ip, total_len);
  datagram->checksum = ~checksum_fold(sum);

  ipv4_send(dest_ip, IPV4_PROTOCOL_UDP, datagram, total_len);
}
