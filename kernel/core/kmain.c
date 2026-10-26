
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

  fs_extent_t extent = {.count = 100};

  fs_alloc_extent(&extent);

  extent.start = 0;
  extent.count = 4;
  fs_free_extent(&extent);

  extent.start = 5;
  extent.count = 5;
  fs_free_extent(&extent);

  extent.start = 11;
  extent.count = 4;
  fs_free_extent(&extent);

  extent.start = 16;
  extent.count = 3;
  fs_free_extent(&extent);

  extent.start = 20;
  extent.count = 2;
  fs_free_extent(&extent);

  extent.start = 23;
  extent.count = 1;
  fs_free_extent(&extent);

  kmemory_dump_bin(fs_get_bitmap(), 0x4);

  extent.count = 10;
  fs_alloc_extent(&extent);
  kmemory_dump_bin(fs_get_bitmap(), 0x4);
  kprintf("start: %u\ncount: %u\n", extent.start, extent.count);
}
