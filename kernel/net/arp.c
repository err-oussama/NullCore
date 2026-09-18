#include "types.h"
#include <arp.h>
#include <eth.h>
#include <kprint.h>
#include <kstring.h>

void arp_dump(arp_t *message) {
  kprintf("Operation: %x\n", message->oper);

  kprintf("Hardware:\n");
  kprintf("  type: %x\n", message->htype);
  kprintf("  len : %x\n", message->hlen);
  kprintf("  Sender Address: %x:%x:%x:%x:%x:%x\n", message->sha[0],
          message->sha[1], message->sha[2], message->sha[3], message->sha[4],
          message->sha[5]);
  kprintf("  Target Address: %x:%x:%x:%x:%x:%x\n", message->tha[0],
          message->tha[1], message->tha[2], message->tha[3], message->tha[4],
          message->tha[5]);

  kprintf("Protocol:\n");
  kprintf("  type: %x\n", message->ptype);
  kprintf("  len : %x\n", message->plen);

  kprintf("  Sender Address: %u.%u.%u.%u\n", message->spa[0], message->spa[1],
          message->spa[2], message->spa[3]);

  kprintf("  Target Address: %u.%u.%u.%u\n", message->tpa[0], message->tpa[1],
          message->tpa[2], message->tpa[3]);
}

void arp_init_msg(arp_t *msg) {
  msg->htype = HTONS(1);
  msg->ptype = ETH_TYPE_IPV4_NET;
  msg->hlen = 6;
  msg->plen = 4;
  eth_get_mac(msg->sha);
  msg->spa[0] = 229; // mock IP address for now
  msg->spa[1] = 123;
  msg->spa[2] = 11;
  msg->spa[3] = 19;
}

void arp_init_reply(arp_t *reply, arp_t *request) {
  arp_init_msg(reply);
  reply->oper = ARP_OPER_REPLY_NET;
  for (uint32 i = 0; i < 6; i++) {
    reply->tha[i] = request->sha[i];
  }
  for (uint32 i = 0; i < 4; i++) {
    reply->tpa[i] = request->spa[i];
  }
}

void arp_init_request(arp_t *request, void *ip) {
  arp_init_msg(request);
  request->oper = ARP_OPER_REQUEST_NET;
  for (uint32 i = 0; i < 4; i++) {
    request->tpa[i] = *(uint8 *)(ip + i);
  }
}

void arp_request(void *ip) {
  arp_t request;
  arp_init_request(&request, ip);
  uint8 dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  eth_send(dest_mac, ETH_TYPE_ARP, &request, sizeof(arp_t));
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
