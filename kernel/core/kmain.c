#include <arp.h>
#include <ata.h>
#include <eth.h>
#include <icmp.h>
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
#include <task.h>
#include <tcp.h>
#include <types.h>
#include <udp.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprintf("==========================="
          "[ NullCore - File System ]"
          "===========================");

  void *buffer = pmm_alloc(1);

  ata_read_sector(0, 1, buffer);

  kprintf("%s", buffer);

  ata_identify(buffer);
}
