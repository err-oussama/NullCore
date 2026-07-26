#include <kernel.h>

#include <ata.h>
#include <pmm.h>
void task_mock() { kprintf("t"); }

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprint_wrn("======== [ Working on: Drive Driver ] ========\n");
  ata_drive_setup();
  ata_identify((void *)pmm_alloc());
}
