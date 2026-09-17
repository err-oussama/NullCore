#include <arp.h>
#include <ata.h>
#include <eth.h>
#include <kernel.h>
#include <kheap.h>
#include <kprint.h>
#include <kstring.h>
#include <multiboot_metadata.h>
#include <pci.h>
#include <pmio.h>
#include <pmm.h>
#include <rtl8139.h>
#include <types.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprintf("=============================[ NullCore - Network "
          "]=============================");
  uint8 dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  pci_rtl8139_get_mac(dest_mac);
  arp_t message;

  message.htype = 10;
  message.ptype = 20;

  message.hlen = 6;
  message.plen = 4;

  message.oper = 1;

  message.sha[0] = 0xAA;
  message.sha[1] = 0xAA;
  message.sha[2] = 0xAA;
  message.sha[3] = 0xAA;
  message.sha[4] = 0xAA;
  message.sha[5] = 0xAA;

  message.spa[0] = 124;
  message.spa[1] = 124;
  message.spa[2] = 124;
  message.spa[3] = 124;

  uint16 payload_len = sizeof(arp_t);

  eth_send(dest_mac, ETH_TYPE_ARP, &message, payload_len);
  eth_poll();
}
