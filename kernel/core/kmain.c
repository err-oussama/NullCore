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

  uint32 ip = 0x08080808;
  arp_request(&ip);
  eth_poll();
}
