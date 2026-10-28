#include "types.h"
#include <ata.h>
#include <fs.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>

static fs_disk_t fs_disk;

static fs_superblock_t fs_superblock;

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
void *fs_get_bitmap() { return fs_bitmap; }

void fs_free_extent(fs_extent_t *extent) {
  for (uint32 i = 0; i < extent->count; i++)
    fs_mark_free(extent->start + i);
}

void fs_alloc_extent(fs_extent_t *extent) {
  extent->start = 0;
  uint32 best_count = 0;
  uint32 count = 0;
  uint32 start = 0;
  for (uint32 sec = fs_data_start;
       sec < fs_disk.sector_count && count < extent->count; sec++) {

    if (fs_is_used(sec))
      continue;

    count = 0;
    start = sec;
    while (sec < fs_disk.sector_count && !fs_is_used(sec) &&
           count < extent->count) {
      count++;
      sec++;
    }

    if (count > best_count) {
      best_count = count;
      extent->start = start;
    }

    if (count >= extent->count)
      break;
  }
  extent->count = best_count;
  for (uint32 i = 0; i < best_count; i++)
    fs_mark_used(extent->start + i);
}

uint32 fs_sectors_needed(uint32 size) {
  return size / fs_disk.sector_size + (size % fs_disk.sector_size ? 1 : 0);
}

void fs_format_disk() {
  fs_superblock_t *superblock = pmm_alloc(1);

  if (!superblock)
    return;
  bezero(superblock, fs_disk.sector_size);

  uint32 sec_c = fs_disk.sector_count;
  uint32 bitmap_bytes = (sec_c / 8) + (sec_c % 8 ? 1 : 0);
  uint32 sec_needed = fs_sectors_needed(bitmap_bytes);

  memcpy(FS_MAGIC, superblock->magic, FS_MAGIC_LEN);
  superblock->version = FS_VERSION;
  superblock->sector_count = fs_disk.sector_count;
  superblock->sector_size = fs_disk.sector_size;
  superblock->bitmap_start = 1;
  superblock->bitmap_size = bitmap_bytes;
  superblock->root_dir = superblock->bitmap_start + sec_needed;
  superblock->data_start = superblock->root_dir + 5;

  ata_write_sector(0, 1, superblock);
  pmm_free(superblock, 1);
}

void fs_init_disk() {
  fs_disk.sector_count = ata_sector_count();
  fs_disk.sector_size = ATA_SECTOR_SIZE;

  uint8 *sector = pmm_alloc(1);
  if (!sector)
    return;

  ata_read_sector(0, 1, sector);
  for (uint32 i = 0; i < FS_MAGIC_LEN; i++) {
    if (sector[i] != FS_MAGIC[i]) {
      fs_format_disk();
      return;
    }
  }
  memcpy(sector, &fs_superblock, sizeof(fs_superblock));
}
