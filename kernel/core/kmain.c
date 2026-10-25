
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

  fs_extent_t extent = {.count = 16};

  fs_alloc_extent(&extent);
  kmemory_dump_bin(fs_get_bitmap(), 0x4);

  extent.start = 0;
  extent.count = 100;
  fs_free_extent(&extent);
  kmemory_dump_bin(fs_get_bitmap(), 0x4);
  extent.count = 100;
  fs_alloc_extent(&extent);
  kmemory_dump_bin(fs_get_bitmap(), 0x4);
  kprintf("start: %u\ncount: %u\n", extent.start, extent.count);
  fs_alloc_extent(&extent);
  kmemory_dump_bin(fs_get_bitmap(), 0x4);
  kprintf("start: %u\ncount: %u\n", extent.start, extent.count);
}
