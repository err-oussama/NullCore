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
  bezero(name, 50);
  for (uint32 a = 0; a < 20; a++) {
    for (uint32 b = 0; b < 28; b++) {
      uint32 half = sizeof(name) / 2;
      memset(name, b + 'A', half);
      memset(name + half, b + 'a', half);
      fs_new_entry("/", name, FS_ENTRY_TYPE_FILE);
    }
  }
}

void one() {

  int8 name[54];
  memset(name, '/', 25);
  memset(name + 25, 'Z', 28);
  fs_new_entry("/", name, FS_ENTRY_TYPE_FILE);
}

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  banner();
  fs_init_disk();

  /* fill_disk(); */
  fs_new_entry("/", "home", FS_ENTRY_TYPE_DIR);
  /* fs_new_entry("/home", "null-core", FS_ENTRY_TYPE_DIR); */
  /* fs_ls("/home"); */
}
