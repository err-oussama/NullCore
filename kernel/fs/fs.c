#include "types.h"
#include <ata.h>
#include <fs.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>

static fs_disk_t fs_disk;
static uint8 *fs_bitmap;

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
