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

void fill_disk() {
  int8 name[50];
  for (uint32 l = 0; l < 20; l++) {
    for (uint32 i = 0; i < 28; i++) {
      uint32 half = sizeof(name) / 2;
      memset(name, i + 'A', half);
      memset(name + half, i + 'a', half);
      fs_new_entry("/", name, FS_ENTRY_TYPE_FILE);
    }
  }
}

void one() {

  int8 name[50];
  uint32 half = sizeof(name) / 2;
  memset(name, 'X', half);
  memset(name + half, 'Z', half);
  fs_new_entry("/", name, FS_ENTRY_TYPE_FILE);
}

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  banner();
  fs_init_disk();

  /* fill_disk(); */
  one();
}
