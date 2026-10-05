#include <byteorder.h>
#include <checksum.h>
#include <ipv4.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>
#include <tcp.h>
#include <types.h>

void tcp_handler(uint32 pseudo_sum, tcp_t *segment, uint16 len) {
  if (!checksum_is_valid(segment, len, pseudo_sum))
    return;
  kprintf("TCP segment\n");
}

void tcp_send(uint8 *dest_ip, uint16 src_port, uint16 dest_port, void *payload,
              uint16 len) {

  uint16 total_len = len + sizeof(tcp_t);
  if (total_len > TCP_MAX_PAYLOAD_SIZE)
    return;

  tcp_t *segment = pmm_alloc(1);
  if (!segment)
    return;

  memcpy(payload, segment->payload, len);
  segment->src_port = htons(src_port);
  segment->dest_port = htons(dest_port);
  segment->checksum = 0;

  uint32 pseudo_sum =
      ipv4_pseudo_sum(ipv4_dev_ip(), dest_ip, IPV4_PROTO_TCP, total_len);
  segment->checksum = checksum_calc(segment, total_len, pseudo_sum);

  ipv4_send(dest_ip, IPV4_PROTO_TCP, segment, total_len);
  pmm_free(segment, 1);
}
