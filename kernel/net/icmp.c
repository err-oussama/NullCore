#include <byteorder.h>
#include <icmp.h>
#include <ipv4.h>
#include <kprint.h>
#include <types.h>

static icmp_echo_session_t echo_sess;

uint32 checksum_word_sum(void *addr, uint16 len) {
  uint16 *words = addr;
  uint32 sum = 0;
  for (uint16 i = 0; i < len / 2; i++)
    sum += words[i];
  if (len & 1)
    sum += ((uint8 *)addr)[len - 1];
  return sum;
}

uint32 checksum_fold(uint32 sum) {
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);
  return sum;
}

uint8 checksum_is_valid(void *addr, uint16 len) {
  return checksum_fold(checksum_word_sum(addr, len)) == 0xFFFF;
}

uint16 checksum_calc(void *addr, uint16 len) {
  return ~checksum_fold(checksum_word_sum(addr, len)) & 0xFFFF;
}

void icmp_handle_echo_request(uint8 *ipv4, icmp_t *msg, uint16 len) {
  msg->checksum = 0;
  msg->type = ICMP_TYPE_ECHO_REPLY;
  msg->checksum = checksum_calc(msg, len);
  ipv4_send(ipv4, IPV4_PROTOCOL_ICMP, msg, len);
}

void icmp_handle_echo_reply(uint8 *ipv4, icmp_t *msg, uint16 len) {
  if (!echo_sess.is_waiting)
    return;
  icmp_echo_t *echo = (icmp_echo_t *)msg;
  if (ntohs(echo->id) != echo_sess.id || ntohs(echo->seq) != echo_sess.seq)
    return;
  kprintf("IP: %u.%u.%u.%u, ID: %u, Seq: %u", ipv4[0], ipv4[1], ipv4[2],
          ipv4[3], echo_sess.id, echo_sess.seq);
  echo_sess.is_waiting = 0;
}
void icmp_handler(uint8 *ipv4, icmp_t *msg, uint16 len) {

  if (!checksum_is_valid(msg, len))
    return;

  switch (msg->type) {
  case ICMP_TYPE_ECHO_REQUEST:
    icmp_handle_echo_request(ipv4, msg, len);
    break;
  case ICMP_TYPE_ECHO_REPLY:
    icmp_handle_echo_reply(ipv4, msg, len);
    break;
  }
}
