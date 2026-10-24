#include "vga_print.h"
#include <ata.h>
#include <fs.h>
#include <kernel.h>
#include <kprint.h>
#include <multiboot_metadata.h>
#include <types.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprintf("==========================="
          "[ NullCore - File System ]"
          "===========================");

  fs_init_disk();
}
