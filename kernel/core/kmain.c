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
#include <types.h>
#include <udp.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprintf("============================="
          "[ NullCore - Network ]"
          "=============================");
  uint8 ip[] = {192, 168, 100, 1};
  /* uint8 ip[] = {10, 122, 93, 201}; */
  /* uint8 ip[] = {172, 18, 0, 1}; */
  /* uint8 ip[] = {172, 17, 0, 1}; */
  /* uint8 data[] = {'Z', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'Z'}; */
  /* icmp_echo_request(ip, data, sizeof(data)); */

  uint8 payload[] = {'X', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'X'};
  udp_send(ip, 12345, 12345, payload, sizeof(payload));

  while (1)
    eth_poll();
}
