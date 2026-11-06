#include <ata.h>
#include <fs.h>
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>
#include <types.h>

static fs_t fs;

static void fs_superblock_dump(fs_superblock_t *superblock) {
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
  kprintf("root dir     : %u\n", superblock->root.index_lba);
  kprintf("root size    : %u\n", superblock->root.size);
  kprintf("data start   : %u\n", superblock->data_start);

  kprintf("------------------------------\n");
}

static void fs_mark_used(uint32 lba) {
  fs.bitmap[lba / 8] |= (0x1 << (lba % 8));
}
static void fs_mark_free(uint32 lba) {
  fs.bitmap[lba / 8] &= ~(0x1 << (lba % 8));
}
static uint8 fs_is_used(uint32 lba) {
  return (fs.bitmap[lba / 8] >> (lba % 8)) & 1;
}

static void fs_free_extent(fs_extent_t *extent) {
  for (uint32 i = 0; i < extent->count; i++)
    fs_mark_free(extent->start + i);
}

static void fs_alloc_extent(fs_extent_t *extent) {
  extent->start = 0;
  uint32 best_count = 0;
  uint32 count = 0;
  uint32 start = 0;
  for (uint32 sec = fs.sb.data_start;
       sec < fs.disk.sector_count && count < extent->count; sec++) {

    if (fs_is_used(sec))
      continue;

    count = 0;
    start = sec;
    while (sec < fs.disk.sector_count && !fs_is_used(sec) &&
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
  return size / fs.disk.sector_size + (size % fs.disk.sector_size ? 1 : 0);
}

static void fs_format() {

  // how many sector the superblock and rood dir will take;
  uint32 fixed_sectors_bytes = fs.disk.sector_size * 2;
  uint32 bitmap_bytes =
      (fs.disk.sector_count / 8) + (fs.disk.sector_count % 8 ? 1 : 0);
  uint32 sec_needed = fs_sectors_needed(bitmap_bytes);
  uint32 frame_count = pmm_frame_for(bitmap_bytes + fixed_sectors_bytes);

  void *buff = pmm_alloc(frame_count);
  if (!buff)
    return;
  bezero(buff, frame_count * PMM_FRAME_SIZE);

  fs_superblock_t *superblock = buff;

  memcpy(FS_MAGIC, superblock->magic, FS_MAGIC_LEN);
  superblock->version = FS_VERSION;
  superblock->sector_count = fs.disk.sector_count;
  superblock->sector_size = fs.disk.sector_size;
  superblock->bitmap_start = 1;
  superblock->bitmap_sectors = sec_needed;
  superblock->bitmap_bytes = bitmap_bytes;
  superblock->root.index_lba = superblock->bitmap_start + sec_needed;
  superblock->root.size = 0;
  superblock->data_start = superblock->root.index_lba + 1;

  fs.bitmap = buff + fs.disk.sector_size;
  for (uint32 i = 0; i < superblock->data_start; i++)
    fs_mark_used(i);

  ata_write_sector(0, superblock->data_start, buff);
  pmm_free(buff, frame_count);
}

void fs_bitmap_load() {
  fs.bitmap_frames = pmm_frame_for(fs.sb.bitmap_bytes);
  fs.bitmap = pmm_alloc(fs.bitmap_frames);
  if (!fs.bitmap)
    kprintf("FS bitmap allocation faild\n");
  ata_read_sector(fs.sb.bitmap_start, fs.sb.bitmap_sectors, fs.bitmap);
}

void fs_bitmap_flush() {
  ata_write_sector(fs.sb.bitmap_start, fs.sb.bitmap_sectors, fs.bitmap);
}

void fs_init_disk() {
  fs.disk.sector_count = ata_sector_count();
  fs.disk.sector_size = ATA_SECTOR_SIZE;

  uint8 *sector = pmm_alloc(1);
  if (!sector)
    return;

  ata_read_sector(0, 1, sector);
  memcpy(sector, &fs.sb, sizeof(fs.sb));
  if (fs.sb.sector_count != fs.disk.sector_count ||
      fs.sb.sector_size != fs.disk.sector_size) {
    fs_format();
    return;
  }
  for (uint32 i = 0; i < FS_MAGIC_LEN; i++) {
    if (fs.sb.magic[i] != FS_MAGIC[i]) {
      fs_format();
      return;
    }
  }
  /* fs_superblock_dump(&(fs.sb)); */
  fs_bitmap_load();
}

void fs_update_sector(uint32 sector, uint16 offset, void *data, uint16 len) {

  void *buff = pmm_alloc(1);
  if (!buff)
    return;

  ata_read_sector(sector, 1, buff);
  memcpy(data, buff + offset, len);
  ata_write_sector(sector, 1, buff);
  fs.sb.root.size += len;
  pmm_free(buff, 1);
}

uint32 fs_get_lba(fs_extent_t *index, uint32 sector) {
  for (uint32 i = 0; index[i].start; i++) {
    uint32 count = index[i].count;
    if (sector < count)
      return index[i].start + sector;
    sector -= count;
  }
  return 0;
}

uint32 fs_add_extent(fs_index_t *index, uint32 index_lba) {

  uint32 i = 0;
  while (index->extents[i].start && i < FS_EXTENTS_MAX)
    i++;
  if (i >= FS_EXTENTS_MAX) {
    fs_extent_t extent;
    extent.count = 1;
    fs_alloc_extent(&extent);
    if (!extent.start)
      return 0;
    fs_index_t *new_index = pmm_alloc(1);
    if (!new_index)
      return 0;
    bezero(new_index, fs.disk.sector_size);
    new_index->extents[0].count = 1;
    fs_alloc_extent(&new_index->extents[0]);
    if (!new_index->extents[0].start)
      return 0;

    ata_write_sector(extent.start, 1, new_index);
    fs_bitmap_flush();
    index->next = extent.start;

    return new_index->extents[0].start;
  } else {
    index->extents[i].count = 1;
    fs_alloc_extent(&index->extents[i]);
    if (!index->extents[i].count)
      return 0;
    fs_bitmap_flush();
  }
  ata_write_sector(index_lba, 1, index);

  return index->extents[i].start;
}

void fs_new_entry(int8 *path, int8 *name, uint8 type) {
  if (memcmp(path, FS_ROOT_PATH, 2) || !fs.sb.root.index_lba)
    return;
  uint32 root_lba = fs.sb.root.index_lba;

  uint32 sector = fs.sb.root.size / fs.disk.sector_size;
  uint32 offset = fs.sb.root.size % fs.disk.sector_size;

  fs_index_t *index = pmm_alloc(1);
  if (!index)
    return;
  ata_read_sector(fs.sb.root.index_lba, 1, index);

  uint32 lba = fs_get_lba(index->extents, sector);
  if (!lba) {
    lba = fs_add_extent(index, fs.sb.root.index_lba);
    if (!lba) {

      kprintf("index is full\n");
      return;
    }
  }

  uint16 name_size = strlen(name) + 1;
  if (name_size > FS_NAME_MAX)
    return;
  fs_entry_t entry;
  bezero(&entry, sizeof(fs_entry_t));
  memcpy(name, entry.name, name_size);
  entry.type = type;

  fs_update_sector(lba, offset, &entry, sizeof(fs_entry_t));
}
