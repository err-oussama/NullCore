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
  uint32 seq_n;
  uint32 ack_n;

  // bits 4-7: header length in 32-bit words; bits 0-3 reserved
  uint8 data_offset;

  // bits 0-5: FIN, SYN, RST, PSH, ACK, URG; bits 6-7: reserved
  uint8 flags;

  // How many bytes of buffer space the sender currently has free to receive
  uint16 window;

  // one's-complement checksum over pseudo-header + TCP header + payload
  uint16 checksum;

  //
  uint16 urgent_ptr;
  uint8 payload[];
} tcp_t;

void tcp_handler(uint32 pseudo_sum, tcp_t *segment, uint16 len);
void tcp_send(uint8 *dest_ip, uint16 src_port, uint16 dest_port, void *payload,
              uint16 len);

#endif
