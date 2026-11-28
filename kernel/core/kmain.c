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

// fs_new_entry("/", "usr", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "var", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "sys", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "dev", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "etc", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "srv", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "mnt", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "opt", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "root", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "run", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "snap", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "proc", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "boot", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "home", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "media", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/", "lost+found", FS_ENTRY_TYPE_DIR);


// fs_new_entry("/home", "err-oussama", FS_ENTRY_TYPE_DIR);

// fs_new_entry("/home/err-oussama", "Work", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/home/err-oussama", "Desktop", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/home/err-oussama", "Pictures", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/home/err-oussama", "Documents", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/home/err-oussama", "Downloads", FS_ENTRY_TYPE_DIR);

// fs_new_entry("/usr", "bin", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/usr", "lib", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/usr", "lib32", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/usr", "lib64", FS_ENTRY_TYPE_DIR);
// fs_new_entry("/usr", "libx32", FS_ENTRY_TYPE_DIR);

// fs_new_entry("/usr/bin", "gcc", FS_ENTRY_TYPE_FILE);
// fs_new_entry("/usr/bin", "gdb", FS_ENTRY_TYPE_FILE);
// fs_new_entry("/usr/bin", "clang", FS_ENTRY_TYPE_FILE);
// fs_new_entry("/usr/bin", "python", FS_ENTRY_TYPE_FILE);

  fs_ls("/usr/bin");
  kprintf("\n");
  fs_ls("/usr");

  kprintf("\n");
  fs_ls("/home");

  kprintf("\n");
  fs_ls("/home/err-oussama");

}
