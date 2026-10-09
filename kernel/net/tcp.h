#ifndef TCP_H
#define TCP_H
#include <types.h>

#define TCP_MAX_PAYLOAD_SIZE 1450

#define TCP_FLAGS_FIN 0x01 // Finish
#define TCP_FLAGS_SYN 0x02 // Synchronize
#define TCP_FLAGS_RST 0x04 // Reset
#define TCP_FLAGS_PSH 0x08 // Push
#define TCP_FLAGS_ACK 0x10 // Acknowledgment
#define TCP_FLAGS_URG 0x20 // Urgent

typedef struct __attribute__((packed)) {

  uint16 src_port;
  uint16 dest_port;

  // position, in the overall byte stream, of the first pyalod byte in this
  // segment, relative to this connection's initial sequence number (ISN)
  uint32 seq_n;
  // next byte this side expects to receive; only meaningful when ACK is set
  uint32 ack_n;

  // bits 4-7: header length in 32-bit words; bits 0-3 reserved
  uint8 header_len;

  // bits 0-5: FIN, SYN, RST, PSH, ACK, URG; bits 6-7: reserved
  uint8 flags;

  // How many bytes of buffer space the sender currently has free to receive
  uint16 window;

  // one's-complement checksum over pseudo-header + TCP header + payload
  uint16 checksum;

  // only meaningful when URG is set;
  // offset from payload[0] marking the end of urgent data in this segment
  uint16 urgent_ptr;

  uint8 payload[];
} tcp_t;

typedef enum {
  TCP_CLOSED,
  TCP_ESTABLISHED,
} tcp_state_e;

typedef struct {
  uint8 remote_ip[4];
  uint8 local_ip[4];

  uint16 remote_port;
  uint16 local_port;

  uint32 remote_ISN;
  uint32 local_ISN;

  uint32 remote_next_seq_n; // next seq_n expected from remote
                            // what i send as ack_n
  uint32 local_next_seq_n;  // next seq_n I'll use when sending data to remote

  uint16 remote_window;
  tcp_state_e state;
  uint8 in_use;

  void *tx_buf;
  void *rx_buf;

  uint16 tx_len;
  uint16 rx_len;
} tcb_t;

void tcp_handler(uint32 pseudo_sum, uint8 *src_ip, uint8 *dest_ip,
                 tcp_t *segment, uint16 len);
void tcp_connect(uint8 *dest_ip, uint16 src_port, uint16 dest_port,
                 void *payload, uint16 len);

void tcp_init_tcb_table();
#endif
