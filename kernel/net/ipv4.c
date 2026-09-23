#include "types.h"
#include <ipv4.h>
#include <kprint.h>

void ipv4_dump(ipv4_t *packet) {
  kprintf("Version   : 0x%x\n", packet->ver_ihl >> 4);
  kprintf("Header Len: 0x%x\n", packet->ver_ihl & 0xF);
}

void ipv4_handler(ipv4_t *packet) { ipv4_dump(packet); }
