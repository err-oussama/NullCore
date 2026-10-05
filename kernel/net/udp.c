#include <byteorder.h>
#include <checksum.h>
#include <ipv4.h>
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

  uint32 pseudo_sum =
      ipv4_pseudo_sum(ipv4_dev_ip(), dest_ip, IPV4_PROTO_UDP, total_len);
  datagram->checksum = checksum_calc(datagram, total_len, pseudo_sum);

  ipv4_send(dest_ip, IPV4_PROTO_UDP, datagram, total_len);
  pmm_free(datagram, 1);
}

void udp_handler(uint32 pseudo_sum, udp_t *datagram, uint16 len) {
  if (!checksum_is_valid(datagram, len, pseudo_sum))
    return;
  udp_dump(datagram);
}
