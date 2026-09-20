#include "types.h"
#include <arp.h>
#include <eth.h>
#include <kprint.h>
#include <kstring.h>
#define ARP_MAC_LEN 6
#define ARP_IPV4_LEN 4

void arp_dump(arp_t *message) {
  kprintf("-- ARP ");
  switch (message->oper) {
  case ARP_OPER_REPLY_NET:
    kprintf("REPLY --\n");
    break;
  case ARP_OPER_REQUEST_NET:
    kprintf("REQUEST --\n");
    break;
  default:
    kprintf("UNKNOWN --\n");
    break;
  }
  kprintf("Sender: ");

  for (uint32 i = 0; i < ARP_MAC_LEN; i++) {
    kprint_hex_padded(message->sha[i], 2);
    if (i < ARP_MAC_LEN - 1)
      kprint_cha(':');
    else
      kprint_cha(' ');
  }
  for (uint32 i = 0; i < ARP_IPV4_LEN; i++)
    kprintf("%u%c", message->spa[i], i < ARP_IPV4_LEN - 1 ? '.' : '\n');

  kprintf("Target: ");

  for (uint32 i = 0; i < ARP_MAC_LEN; i++) {
    kprint_hex_padded(message->tha[i], 2);
    if (i < ARP_MAC_LEN - 1)
      kprint_cha(':');
    else
      kprint_cha(' ');
  }
  for (uint32 i = 0; i < ARP_IPV4_LEN; i++)
    kprintf("%u%c", message->tpa[i], i < ARP_IPV4_LEN - 1 ? '.' : '\n');
}

void arp_init_msg(arp_t *msg) {
  msg->htype = HTONS(1);
  msg->ptype = ETH_TYPE_IPV4_NET;
  msg->hlen = ARP_MAC_LEN;
  msg->plen = ARP_IPV4_LEN;
  eth_get_mac(msg->sha);
  msg->spa[0] = 12; // mock IP address for now
  msg->spa[1] = 34;
  msg->spa[2] = 56;
  msg->spa[3] = 78;
}

void arp_init_reply(arp_t *reply, arp_t *request) {
  arp_init_msg(reply);
  reply->oper = ARP_OPER_REPLY_NET;
  for (uint32 i = 0; i < ARP_MAC_LEN; i++) {
    reply->tha[i] = request->sha[i];
  }
  for (uint32 i = 0; i < ARP_IPV4_LEN; i++) {
    reply->tpa[i] = request->spa[i];
  }
}

void arp_init_request(arp_t *request, void *ip) {
  arp_init_msg(request);
  request->oper = ARP_OPER_REQUEST_NET;
  for (uint32 i = 0; i < ARP_IPV4_LEN; i++) {
    request->tpa[i] = *(uint8 *)(ip + i);
  }
}

void arp_request(void *ip) {
  arp_t request;
  arp_init_request(&request, ip);
  uint8 dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  memset(request.tha, 0, ARP_MAC_LEN);
  eth_send(dest_mac, ETH_TYPE_ARP, &request, sizeof(arp_t));
  arp_dump(&request);
}

void arp_reply(arp_t *request) {
  arp_t reply;
  arp_init_reply(&reply, request);
  eth_send(request->sha, ETH_TYPE_ARP, &reply, sizeof(arp_t));
}

void arp_handler(arp_t *message) {
  arp_dump(message);
  switch (message->oper) {
  case ARP_OPER_REQUEST_NET:
    arp_reply(message);
    break;
  case ARP_OPER_REPLY_NET:
    // arp_save_ip();
    break;
  }
}
