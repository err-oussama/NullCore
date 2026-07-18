#include <kernel.h>

void task_mock() { kprintf("t"); }

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprint_wrn("======== [ Working on: File System ] ========\n");
}
