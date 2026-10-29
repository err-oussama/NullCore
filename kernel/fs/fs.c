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

void fs_format() {

  // how many sector the superblock and rood dir will take;
  uint32 fixed_sectors_bytes = fs_disk.sector_size * 2;
  uint32 bitmap_bytes =
      (fs_disk.sector_count / 8) + (fs_disk.sector_count % 8 ? 1 : 0);
  uint32 sec_needed = fs_sectors_needed(bitmap_bytes);
  uint32 frame_count = (bitmap_bytes / PMM_FRAME_SIZE);
  frame_count += (bitmap_bytes % PMM_FRAME_SIZE) ? 1 : 0;
  frame_count += fixed_sectors_bytes / PMM_FRAME_SIZE;
  frame_count += fixed_sectors_bytes % PMM_FRAME_SIZE ? 1 : 0;

  void *buff = pmm_alloc(frame_count);
  if (!buff)
    return;
  bezero(buff, frame_count * PMM_FRAME_SIZE);

  fs_superblock_t *superblock = buff;

  memcpy(FS_MAGIC, superblock->magic, FS_MAGIC_LEN);
  superblock->version = FS_VERSION;
  superblock->sector_count = fs_disk.sector_count;
  superblock->sector_size = fs_disk.sector_size;
  superblock->bitmap_start = 1;
  superblock->bitmap_bytes = bitmap_bytes;
  superblock->root_dir = superblock->bitmap_start + sec_needed;
  superblock->data_start = superblock->root_dir + 1;

  fs_bitmap = buff + fs_disk.sector_size;
  for (uint32 i = 0; i < superblock->data_start; i++)
    fs_mark_used(i);

  ata_write_sector(0, superblock->data_start, buff);
  pmm_free(buff, frame_count);
}

void fs_superblock_dump(fs_superblock_t *superblock) {
  kprintf("----------superblock----------\n");
  kprintf("magic        : ");
  for (uint32 i = 0; i < FS_MAGIC_LEN; i++)
    kprint_cha(superblock->magic[i]);

  kprintf("\n");
  kprintf("version      : %u\n", superblock->version);
  kprintf("sector count : %u\n", superblock->sector_count);
  kprintf("sector size  : %u\n", superblock->sector_size);
  kprintf("bitmap start : %u\n", superblock->bitmap_start);
  kprintf("bitmap bytes : %u\n", superblock->bitmap_bytes);
  kprintf("root dir     : %u\n", superblock->root_dir);
  kprintf("data start   : %u\n", superblock->data_start);

  kprintf("------------------------------\n");
}

void fs_init_disk() {
  fs_disk.sector_count = ata_sector_count();
  fs_disk.sector_size = ATA_SECTOR_SIZE;

  uint8 *sector = pmm_alloc(1);
  if (!sector)
    return;

  ata_read_sector(0, 1, sector);
  memcpy(sector, &fs_superblock, sizeof(fs_superblock));
  if (fs_superblock.sector_count != fs_disk.sector_count ||
      fs_superblock.sector_size != fs_disk.sector_size) {
    fs_format();
    return;
  }
  for (uint32 i = 0; i < FS_MAGIC_LEN; i++) {
    if (fs_superblock.magic[i] != FS_MAGIC[i]) {
      fs_format();
      return;
    }
  }
  fs_superblock_dump(&fs_superblock);
}
