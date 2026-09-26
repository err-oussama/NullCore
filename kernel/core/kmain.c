#include <arp.h>
#include <ata.h>
#include <eth.h>
#include <ipv4.h>
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
  kprintf("============================="
          "[ NullCore - Network ]"
          "=============================");
  ipv4_init_iface();
  uint8 ip[] = {192, 168, 100, 1};
  ipv4_send(ip, IPV4_PROTOCOL_ICMP, NULL, 0);
  while (1)
    eth_poll();
}
