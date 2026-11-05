#include <ata.h>
#include <fs.h>
#include <kernel.h>
#include <kprint.h>
#include <kstring.h>
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

  int8 name[50];
  for (uint32 i = 0; i < 28; i++) {
    memset(name, i + 'A', sizeof(name) / 2);
    memset(name + sizeof(name) / 2, i + 'a', sizeof(name) / 2);
    fs_new_entry("/", name, FS_ENTRY_TYPE_FILE);
  }
}
