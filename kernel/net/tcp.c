#include <byteorder.h>
#include <checksum.h>
#include <ipv4.h>
#include <kprint.h>
#include <kstring.h>
#include <pit.h>
#include <pmm.h>
#include <tcp.h>
#include <types.h>

#define TCP_MAX_TCBS 10

static tcb_t tcb_table[TCP_MAX_TCBS];

void tcp_init_tcb_table() {
  for (uint32 i = 0; i < TCP_MAX_TCBS; i++) {
    tcb_table[i].tx_buf = pmm_alloc(1);
    tcb_table[i].rx_buf = pmm_alloc(1);

    if (!tcb_table[i].tx_buf || !tcb_table[i].rx_buf) {
      kprintf("[TCP error]: faild to allocate buffers");
      return;
    }
  }
}

void tcp_dump(tcp_t *segment, uint16 len) {
  uint16 src_port = ntohs(segment->src_port);
  uint16 dest_port = ntohs(segment->dest_port);
  uint32 seq_n = ntohl(segment->seq_n);
  uint32 ack_n = ntohl(segment->ack_n);
  uint16 win = ntohs(segment->window);
  uint16 urg = ntohs(segment->urgent_ptr);
  uint16 checksum = ntohs(segment->checksum);

  uint8 hlen = (segment->header_len >> 4) * 4;
  uint8 f = segment->flags;

  kprintf("------ TCP Segment ------\n");

  kprintf("Port: %u -> %u, HdrLen=%u, PayloadLen=%u\n", src_port, dest_port,
          hlen, len);
  kprintf("Seq=%u, Ack=%u, Win=%u, Urg=%u\n", seq_n, ack_n, win, urg);
  kprintf("Flags=[ %s%s%s%s%s%s], checksum: %u\n",
          (f & TCP_FLAGS_ACK) ? "ACK " : "", (f & TCP_FLAGS_SYN) ? "SYN " : "",
          (f & TCP_FLAGS_FIN) ? "FIN " : "", (f & TCP_FLAGS_RST) ? "RST " : "",
          (f & TCP_FLAGS_PSH) ? "PSH " : "", (f & TCP_FLAGS_URG) ? "URG " : "",
          checksum);

  kprintf("-------------------------\n");
}

tcb_t *tcp_get_tcb(uint8 *src_ip, uint8 *dest_ip, uint16 src_port,
                   uint16 dest_port) {
  for (uint32 i = 0; i < TCP_MAX_TCBS; i++) {
    if (memcmp(src_ip, tcb_table[i].remote_ip, 4) &&
        memcmp(dest_ip, tcb_table[i].local_ip, 4) &&
        src_port == tcb_table[i].local_port &&
        dest_port == tcb_table[i].remote_port)
      return &tcb_table[i];
  }

  return NULL;
}

uint32 tcp_gen_ISN() { return (pit_get_tick() >> 4) * pit_get_tick() / 2; }

tcb_t *tcp_setup_tcb(uint8 *local_ip, uint8 *remote_ip, uint16 local_port,
                     uint16 remote_port) {
  for (uint32 i = 0; i < TCP_MAX_TCBS; i++) {
    if (tcb_table[i].in_use)
      continue;
    tcb_table[i].local_port = local_port;
    tcb_table[i].remote_port = remote_port;
    memcpy(remote_ip, tcb_table[i].remote_ip, 4);
    memcpy(local_ip, tcb_table[i].local_ip, 4);
    tcb_table[i].local_ISN = tcp_gen_ISN();
    tcb_table[i].remote_ISN = 0;
    tcb_table[i].local_next_seq_n = tcb_table[i].local_ISN;
    tcb_table[i].remote_next_seq_n = 0;
    tcb_table[i].remote_window = 0;
    tcb_table[i].state = TCP_CLOSED;
    tcb_table[i].rx_len = 0;
    tcb_table[i].in_use = 1;
    return &tcb_table[i];
  }
  return NULL;
}

void tcp_connect(uint8 *dest_ip, uint16 src_port, uint16 dest_port,
                 void *payload, uint16 len) {
  if (len > TCP_MAX_PAYLOAD_SIZE)
    return;

  tcb_t *conn = tcp_setup_tcb(ipv4_dev_ip(), dest_ip, src_port, dest_port);
  if (!conn)
    return;
  memcpy(payload, conn->tx_buf, len);
  conn->tx_len = len;
}
void tcp_handler(uint32 pseudo_sum, uint8 *src_ip, uint8 *dest_ip,
                 tcp_t *segment, uint16 len) {
  if (!checksum_is_valid(segment, len, pseudo_sum))
    return;
  /* tcp_dump(segment, len); */

  if (segment->flags == TCP_FLAGS_SYN) {
    uint16 src_port = ntohs(segment->src_port);
    uint16 dest_port = ntohs(segment->src_port);
  }
}
