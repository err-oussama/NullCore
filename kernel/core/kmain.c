
#include <ata.h>
#include <fs.h>
#include <kernel.h>
#include <kprint.h>
#include <multiboot_metadata.h>
#include <types.h>

void banner() {
  kprintf("==========================="
          "[ NullCore - File System ]"
          "===========================");
}

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  banner();

  fs_init_disk();

  fs_new_entry("/", "AAAAAAAAAAAAAAAAAAAAAAAAA", FS_ENTRY_TYPE_FILE);
  fs_new_entry("/", "BBBBBBBBBBBBBBBBBBBBBBBBB", FS_ENTRY_TYPE_FILE);
  fs_new_entry("/", "CCCCCCCCCCCCCCCCCCCCCCCCC", FS_ENTRY_TYPE_FILE);
}
