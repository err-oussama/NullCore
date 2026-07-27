#include "types.h"
#include <ata.h>
#include <kernel.h>
#include <kstring.h>
#include <pmm.h>
void task_mock() { kprintf("t"); }

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprint_wrn("======== [ Working on: Drive Driver ] ========\n");
  ata_drive_setup();
  ata_identify((void *)pmm_alloc());
  uint8 *read_buffer = (uint8 *)pmm_alloc();
  uint8 *write_buffer = (uint8 *)pmm_alloc();
  char *data = "[Data] going in/out the drive";
  bezero(read_buffer, 0x1000);
  bezero(write_buffer, 0x1000);
  memcpy(data, write_buffer, strlen(data));
  ata_write_sector(0, 1, write_buffer);
  ata_read_sector(0, 1, read_buffer);
  kprintf("%s", read_buffer);
}
