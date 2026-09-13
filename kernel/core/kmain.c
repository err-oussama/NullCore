#include "ethernet.h"
#include "kprint.h"
#include <ata.h>
#include <kernel.h>
#include <kheap.h>
#include <kstring.h>
#include <multiboot_metadata.h>
#include <pci.h>
#include <pmio.h>
#include <pmm.h>
#include <rtl8139.h>
#include <types.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprint_wrn("========[ Working on: Network ]========\n");
  pci_rtl8139_init();
  uint8 mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  uint8 *payload = pmm_alloc(1);
  uint16 payload_len = 82;
  for (uint8 i = 0; i < 10; i++) {
    for (uint8 j = 0; j < payload_len; j++) {
      payload[j] = 'A' + i;
    }
    eth_send(mac, ETH_TYPE_ARP, payload, payload_len);
  }
}
