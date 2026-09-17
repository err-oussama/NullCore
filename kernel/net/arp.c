#include <arp.h>
#include <kprint.h>

void arp_handler(arp_t *message) {
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
