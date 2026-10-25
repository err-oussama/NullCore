#include "types.h"
#include <ata.h>
#include <fs.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>

static fs_disk_t fs_disk;
static uint8 *fs_bitmap;

static uint32 fs_data_start = 0;

static void fs_mark_used(uint32 lba) {
  fs_bitmap[lba / 8] |= (0x1 << (lba % 8));
}
static void fs_mark_free(uint32 lba) {
  fs_bitmap[lba / 8] &= ~(0x1 << (lba % 8));
}
static uint8 fs_is_used(uint32 lba) {
  return (fs_bitmap[lba / 8] >> (lba % 8)) & 1;
}

void fs_init_disk() {
  fs_disk.sector_count = ata_sector_count();
  fs_disk.sector_size = ATA_SECTOR_SIZE;

  fs_bitmap = pmm_alloc(1);
  if (fs_bitmap)
    ata_read_sector(0, 2, fs_bitmap);
  memset(fs_bitmap, 0, 0x1000);
}

void *fs_get_bitmap() { return fs_bitmap; }

void fs_free_extent(fs_extent_t *extent) {
  for (uint32 i = 0; i < extent->count; i++)
    fs_mark_free(extent->start + i);
}

void fs_alloc_extent(fs_extent_t *extent) {
  extent->start = 0;
  uint32 count = 0;
  for (uint32 sec = fs_data_start;
       sec < fs_disk.sector_count && count < extent->count; sec++) {

    if (fs_is_used(sec))
      continue;

    count = 0;
    extent->start = sec;
    while (sec < fs_disk.sector_count && !fs_is_used(sec) &&
           count < extent->count) {
      count++;
      sec++;
    }
    if (count == extent->count)
      break;
  }
  extent->count = count;
  for (uint32 i = 0; i < count; i++)
    fs_mark_used(extent->start + i);
}
