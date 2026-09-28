#include "types.h"
#include <arp.h>
#include <eth.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>

#define ARP_MAC_LEN 6
#define ARP_IPV4_LEN 4
#define ARP_CACHE_SIZE 0x10
#define ARP_QUEUE_SIZE 0x10

static arp_cache_entry_t arp_cache[ARP_CACHE_SIZE];
static arp_pending_t arp_queue[ARP_QUEUE_SIZE];

void arp_dump(arp_t *message) {
  return;
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
  msg->spa[0] = 192; // mock IP address for now
  msg->spa[1] = 168;
  msg->spa[2] = 100;
  msg->spa[3] = 2;
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
  arp_dump(reply);
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

void arp_cache_insert(arp_t *message) {
  for (uint32 i = 0; i < ARP_CACHE_SIZE; i++) {
    if (!arp_cache[i].in_use) {
      for (uint32 j = 0; j < ARP_MAC_LEN; j++) {
        arp_cache[i].mac[j] = message->sha[j];
      }
      for (uint32 j = 0; j < ARP_IPV4_LEN; j++) {
        arp_cache[i].ip[j] = message->spa[j];
      }
      arp_cache[i].in_use = 1;
      return;
    }
  }
}

void *arp_cache_lookup(uint8 *ipv4) {
  for (uint32 i = 0; i < ARP_CACHE_SIZE; i++) {
    if (*(uint32 *)ipv4 == *(uint32 *)(&arp_cache[i].ip)) {
      return arp_cache[i].mac;
    }
  }
  return NULL;
}

void arp_flush_pending(uint8 *ipv4) {
  for (uint32 i = 0; i < ARP_QUEUE_SIZE; i++) {
    if (!arp_queue[i].in_use)
      continue;
    if (*(uint32 *)arp_queue[i].ip == *(uint32 *)ipv4) {
      uint8 *mac = arp_cache_lookup(ipv4);
      eth_send(mac, ETH_TYPE_IPV4, arp_queue[i].packet, arp_queue[i].size);
      arp_queue[i].in_use = 0;
      pmm_free(arp_queue[i].packet, 1);
    }
  }
}
void arp_handler(arp_t *message) {
  arp_dump(message);
  switch (message->oper) {
  case ARP_OPER_REQUEST_NET:
    arp_reply(message);
    break;
  case ARP_OPER_REPLY_NET:
    arp_cache_insert(message);
    arp_flush_pending(message->spa);
    break;
  }
}

void arp_enqueue(uint8 *ipv4, void *packet, uint16 size) {
  for (uint32 i = 0; i < ARP_QUEUE_SIZE; i++) {
    if (!arp_queue[i].in_use) {
      arp_queue[i].size = size;
      arp_queue[i].packet = packet;
      arp_queue[i].in_use = 1;
      memcpy(ipv4, arp_queue[i].ip, 4);
      break;
    }
  }
}

void arp_send_ipv4_packet(uint8 *ipv4, void *packet, uint16 size) {
  uint8 *mac = arp_cache_lookup(ipv4);

  if (mac) {
    eth_send(mac, ETH_TYPE_IPV4, packet, size);
    pmm_free(packet, 1);
  } else {
    arp_request(ipv4);
    arp_enqueue(ipv4, packet, size);
  }
}
