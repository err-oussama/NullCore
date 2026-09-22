#include "types.h"
#include <ipv4.h>
#include <kprint.h>

void ipv4_handler(ipv4_t *packet) {
  kprintf("Protocol: %x\n", packet->protocol);
  for (uint32 i = 0; i < 4; i++) {
    kprintf("%i%c", packet->src_ip[i], i < 3 ? '.' : '\n');
  }
  for (uint32 i = 0; i < 4; i++) {
    kprintf("%i%c", packet->dest_ip[i], i < 3 ? '.' : '\n');
  }
}
