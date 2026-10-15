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

uint32 tcp_gen_ISN() { return pit_get_tick(); }

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

  kprintf("port: %u -> %u, hdr_len=%u, payload_len=%u\n", src_port, dest_port,
          hlen, len);
  kprintf("seq=%u, ack=%u, win=%u, urg=%u\n", seq_n, ack_n, win, urg);
  kprintf("flags=[ %s%s%s%s%s%s], checksum: %u\n",
          (f & TCP_FLAGS_ACK) ? "ACK " : "", (f & TCP_FLAGS_SYN) ? "SYN " : "",
          (f & TCP_FLAGS_FIN) ? "FIN " : "", (f & TCP_FLAGS_RST) ? "RST " : "",
          (f & TCP_FLAGS_PSH) ? "PSH " : "", (f & TCP_FLAGS_URG) ? "URG " : "",
          checksum);

  kprintf("\n");
  for (uint32 i = 0; i < len; i++) {
    kprintf("%c", segment->payload[i]);
  }
  kprintf("\n");

  kprintf("-------------------------\n");
}

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
    tcb_table[i].state = TCP_LISTEN;
    tcb_table[i].rx_len = 0;
    tcb_table[i].in_use = 1;
    return &tcb_table[i];
  }
  return NULL;
}

tcb_t *tcp_get_tcb(uint8 *local_ip, uint8 *remote_ip, uint16 local_port,
                   uint16 remote_port) {
  for (uint32 i = 0; i < TCP_MAX_TCBS; i++) {
    if (tcb_table[i].in_use) {
      if (!memcmp(remote_ip, tcb_table[i].remote_ip, 4) &&
          !memcmp(local_ip, tcb_table[i].local_ip, 4) &&
          local_port == tcb_table[i].local_port &&
          remote_port == tcb_table[i].remote_port)
        return &tcb_table[i];
    }
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

void tcp_init_segment(tcb_t *conn, tcp_t *segment, uint16 len, uint8 flags,
                      uint32 seq_n, uint32 ack_n) {
  segment->src_port = htons(conn->local_port);
  segment->dest_port = htons(conn->remote_port);

  segment->seq_n = htonl(seq_n);
  segment->ack_n = htonl(ack_n);

  segment->header_len = 5 << 4;
  segment->flags = flags;
  segment->window = htons(0x1000);

  segment->checksum = 0;
  segment->urgent_ptr = 0;

  uint32 pseudo_sum =
      ipv4_pseudo_sum(conn->local_ip, conn->remote_ip, IPV4_PROTO_TCP, len);
  segment->checksum = checksum_calc(segment, sizeof(tcp_t), pseudo_sum);
}

void tcp_listen_state_handler(tcb_t *conn, tcp_t *segment, uint16 len) {
  if (segment->flags & TCP_FLAGS_RST) {
    conn->in_use = 0;
  }

  if (segment->flags != TCP_FLAGS_SYN)
    return;
  conn->remote_window = ntohs(segment->window);
  conn->remote_ISN = ntohl(segment->seq_n);

  tcp_t seg;
  len = sizeof(tcp_t);
  uint8 flags = TCP_FLAGS_ACK | TCP_FLAGS_SYN;
  uint32 seq_n = conn->local_ISN;
  uint32 ack_n = conn->remote_ISN + 1;
  tcp_init_segment(conn, &seg, len, flags, seq_n, ack_n);

  conn->local_next_seq_n = conn->local_ISN + 1;
  conn->remote_next_seq_n = conn->remote_ISN + 1;
  conn->state = TCP_SYN_RECEIVED;
  ipv4_send(conn->remote_ip, IPV4_PROTO_TCP, &seg, sizeof(tcp_t));
}

void tcp_syn_reseived_state_handler(tcb_t *conn, tcp_t *segment, uint16 len) {

  uint32 seq_n = ntohl(segment->seq_n);
  uint32 ack_n = ntohl(segment->ack_n);

  if (segment->flags != TCP_FLAGS_ACK)
    return;

  conn->state = TCP_ESTABLISHED;
}

void tcp_established_state_handler(tcb_t *conn, tcp_t *segment, uint16 len) {
  // NOTE: this only handle receving, it does not store data, will implement
  // 			 soon.

  uint16 hlen = (segment->header_len >> 4) * 4;

  if (segment->flags & TCP_FLAGS_RST) {
    conn->in_use = 0;
    return;
  }

  if (!(segment->flags & TCP_FLAGS_ACK))
    return;

  if (len == hlen && !(segment->flags & TCP_FLAGS_FIN))
    return;

  tcp_t seg;

  uint32 size = sizeof(tcp_t);
  uint32 seq_n = conn->local_next_seq_n;
  uint32 ack_n = ntohl(segment->seq_n);
  uint8 flags = TCP_FLAGS_ACK;

  if (segment->flags & TCP_FLAGS_FIN) {
    flags |= TCP_FLAGS_FIN;
    ack_n += 1;

    conn->remote_next_seq_n++;
    conn->local_next_seq_n++;
    conn->state = TCP_LAST_ACK;
  } else {
    ack_n += len - hlen;
    conn->remote_next_seq_n += len - hlen;
  }

  tcp_init_segment(conn, &seg, size, flags, seq_n, ack_n);
  ipv4_send(conn->remote_ip, IPV4_PROTO_TCP, &seg, size);
}

void tcp_last_ack_state_handler(tcb_t *conn, tcp_t *segment, uint16 len) {

  if (segment->flags & TCP_FLAGS_ACK || segment->flags & TCP_FLAGS_RST) {
    conn->in_use = 0;
  }
}
void tcp_handler(uint32 pseudo_sum, uint8 *src_ip, uint8 *dest_ip,
                 tcp_t *segment, uint16 len) {
  if (!checksum_is_valid(segment, len, pseudo_sum))
    return;
  uint16 remote_port = ntohs(segment->src_port);
  uint16 local_port = ntohs(segment->dest_port);
  tcb_t *conn = tcp_get_tcb(dest_ip, src_ip, local_port, remote_port);
  if (!conn) {
    if (segment->flags == TCP_FLAGS_SYN)
      conn = tcp_setup_tcb(dest_ip, src_ip, local_port, remote_port);
    if (!conn)
      return;
  }

  switch (conn->state) {
  case TCP_LISTEN:
    tcp_listen_state_handler(conn, segment, len);
    break;
  case TCP_SYN_RECEIVED:
    tcp_syn_reseived_state_handler(conn, segment, len);
    break;
  case TCP_ESTABLISHED:
    tcp_established_state_handler(conn, segment, len);
    break;
  case TCP_LAST_ACK:
    tcp_last_ack_state_handler(conn, segment, len);
    break;
  default:
    kprintf("Unhandled state\n");
  }
}
