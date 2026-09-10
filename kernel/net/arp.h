#ifndef ARP_H
#define ARP_H

#include <types.h>

typedef struct __attribute__((packed)) {

  uint16 htype; // hardware type
  uint16 ptype; // protocol type

  uint8 hlen; // hardware length
  uint8 plen; // protocol length

  uint16 oper; // operation request; 1=APR request, 2=APR reply.

  uint8 sha[6]; // sender hardware address; sender MAC address
  uint32 spa;   // sender protocol address; sender IP  address

  uint8 tha[6]; // target hardware address; target MAC address
  uint32 tpa;   // target protocol address; target IP  address

} arp_t;

#endif
