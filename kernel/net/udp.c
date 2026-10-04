#include "ipv4.h"
#include <byteorder.h>
#include <checksum.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>
#include <types.h>
#include <udp.h>

void udp_dump(udp_t *datagram) {
  uint16 src_port = ntohs(datagram->src_port);
  uint16 dest_port = ntohs(datagram->dest_port);
  uint16 len = ntohs(datagram->len);
  kprintf("----- UDP -----\n");
  kprintf("src: %u, dest: %u, len: %u\n[", src_port, dest_port, len);

  for (uint16 i = 0; i < ntohs(datagram->len) - sizeof(udp_t); i++)
    kprintf("%c", datagram->payload[i]);

  kprintf("]\n---------------\n");
}

uint8 udp_is_valid(uint32 pseudo_sum, udp_t *datagram, uint16 len) {
  return 0xFFFF == checksum_fold(checksum_word_sum(datagram, len) + pseudo_sum);
}

uint16 udp_calc_checksum(uint8 *dest_ip, udp_t *datagram, uint16 len) {
  return ~checksum_fold(
      checksum_word_sum(datagram, len) +
      ipv4_pseudo_sum(ipv4_get_dev_ip(), dest_ip, IPV4_PROTOCOL_UDP, len));
}

void udp_send(uint8 *dest_ip, uint16 src_port, uint16 dest_port, void *payload,
              uint16 len) {
  uint16 total_len = len + sizeof(udp_t);
  if (total_len > UDP_MAX_PAYLOAD_SIZE)
    return;

  udp_t *datagram = pmm_alloc(1);
  if (!datagram)
    return;

  datagram->checksum = 0;
  datagram->len = htons(total_len);
  datagram->src_port = htons(src_port);
  datagram->dest_port = htons(dest_port);
  memcpy(payload, datagram->payload, len);
  datagram->checksum = udp_calc_checksum(dest_ip, datagram, total_len);

  ipv4_send(dest_ip, IPV4_PROTOCOL_UDP, datagram, total_len);
  pmm_free(datagram, 1);
}

void udp_handler(uint32 pseudo_sum, udp_t *datagram, uint16 len) {
  if (!udp_is_valid(pseudo_sum, datagram, len))
    return;
  udp_dump(datagram);
}
