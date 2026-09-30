#include <byteorder.h>
#include <checksum.h>
#include <icmp.h>
#include <ipv4.h>
#include <kprint.h>
#include <pmm.h>
#include <types.h>

static icmp_echo_session_t echo_sess = {.id = 1, .seq = 1, .is_waiting = 0};

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
  kprintf("IP: %u.%u.%u.%u, ID: %u, Seq: %u, data: ", ipv4[0], ipv4[1], ipv4[2],
          ipv4[3], echo_sess.id, echo_sess.seq);
  for (uint16 i = 0; i < len - sizeof(icmp_echo_t); i++) {
    kprintf("%c", echo->data[i]);
  }
  kprintf("\n");
  echo_sess.is_waiting = 0;
  echo_sess.seq++;
}

void icmp_echo_request(uint8 *ipv4, void *data, uint16 len) {
  if (echo_sess.is_waiting)
    return;
  icmp_echo_t *request = pmm_alloc(1);
  if (!request)
    return;
  request->icmp.type = ICMP_TYPE_ECHO_REQUEST;
  request->icmp.code = 0;
  request->icmp.checksum = 0;
  request->id = htons(echo_sess.id);
  request->seq = htons(echo_sess.seq);
  echo_sess.is_waiting = 1;
  for (uint16 i = 0; i < len; i++) {
    request->data[i] = *(uint8 *)(data + i);
  }
  request->icmp.checksum = checksum_calc(request, len + sizeof(icmp_echo_t));
  ipv4_send(ipv4, IPV4_PROTOCOL_ICMP, request, len + sizeof(icmp_echo_t));
  pmm_free(request, 1);
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
