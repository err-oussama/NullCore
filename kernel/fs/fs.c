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

void fs_bitmap_load() {
  fs.bitmap_frames = pmm_frame_for(fs.sb.bitmap_bytes);
  fs.bitmap = pmm_alloc(fs.bitmap_frames);
  if (!fs.bitmap)
    kprintf("FS bitmap allocation faild\n");
  ata_read_sectors(fs.sb.bitmap_start, fs.sb.bitmap_sectors, fs.bitmap);
}

void fs_bitmap_flush() {
  ata_write_sectors(fs.sb.bitmap_start, fs.sb.bitmap_sectors, fs.bitmap);
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

  ata_write_sectors(0, superblock->data_start, buff);
  pmm_free(buff, frame_count);

  // TODO: remove this later
  uint8 *sector = pmm_alloc(1);
  if (!sector)
    return;

  ata_read_sector(0, sector);
  memcpy(sector, &fs.sb, sizeof(fs.sb));
  fs_bitmap_load();
}

void fs_init_disk() {
  fs.disk.sector_count = ata_sector_count();
  fs.disk.sector_size = ATA_SECTOR_SIZE;

  uint8 *sector = pmm_alloc(1);
  if (!sector)
    return;

  ata_read_sector(0, sector);
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
}

uint32 fs_update_sector(uint32 entry_lba, uint32 entry_offset, uint32 sector,
                        uint16 offset, void *data, uint16 len) {

  void *buff = pmm_alloc(1);
  if (!buff)
    return 1;

  ata_read_sector(sector, buff);
  memcpy(data, buff + offset, len);
  ata_write_sector(sector, buff);

  ata_read_sector(entry_lba, buff);
  fs_entry_t *entry = buff + entry_offset;
  entry->size += len;
  ata_write_sector(entry_lba, buff);
  if (!entry_lba)
    memcpy(entry, &fs.sb.root, sizeof(fs_entry_t));

  pmm_free(buff, 1);
  return 0;
}

uint32 fs_index_lookup(fs_index_t *index, uint32 sector) {

  while (1) {
    for (uint32 i = 0; index->extents[i].start; i++) {
      uint32 count = index->extents[i].count;
      if (sector < count) {
        return index->extents[i].start + sector;
      }
      sector -= count;
    }
    if (!index->next)
      break;
    ata_read_sector(index->next, index);
  }
  return 0;
}

uint32 fs_index_grow(fs_index_t *index, uint32 index_lba) {

  uint32 i;
  while (1) {
    i = 0;
    while (index->extents[i].start && i < FS_EXTENTS_MAX)
      i++;
    if (!index->next)
      break;
    index_lba = index->next;
    ata_read_sector(index_lba, index);
  }
  if (i >= FS_EXTENTS_MAX) {
    fs_extent_t extent;
    extent.count = 1;
    fs_alloc_extent(&extent);
    if (!extent.start)
      return 0;
    index->next = extent.start;
    ata_write_sector(index_lba, index);
    fs_bitmap_flush();
    bezero(index, fs.disk.sector_size);
    index_lba = extent.start;
    i = 0;
  }
  index->extents[i].count = 1;
  fs_alloc_extent(&index->extents[i]);
  if (!index->extents[i].count)
    return 0;
  ata_write_sector(index_lba, index);
  fs_bitmap_flush();
  return index->extents[i].start;
}

uint32 fs_map_sector(uint32 index_lba, uint32 sec_id) {
  fs_index_t *index = pmm_alloc(1);
  if (!index)
    return 0;

  ata_read_sector(index_lba, index);
  uint32 lba = fs_index_lookup(index, sec_id);
  if (!lba) {
    ata_read_sector(index_lba, index);
    lba = fs_index_grow(index, index_lba);
  }
  pmm_free(index, 1);
  return lba;
}

void fs_resolve(int8 *path, fs_node_t *node) {
  memcpy(&fs.sb.root, node, sizeof(fs_entry_t));
  node->entry_lba = 0;
  node->entry_offset = (void *)(&fs.sb.root) - (void *)(&fs.sb);
}

void fs_write(int8 *path, uint32 offset, void *data, uint32 len) {
  uint32 sector_index = offset / fs.disk.sector_size;
  uint32 sector_offset = offset % fs.disk.sector_size;

  fs_node_t node;
  fs_resolve(path, &node);

  uint32 lba = fs_map_sector(node.entry.index_lba, sector_index);
  if (!lba)
    return;
  if (fs_update_sector(node.entry_lba, node.entry_offset, lba, sector_offset,
                       data, len))
    return;
}

uint32 fs_read(int8 *path, uint32 offset, void *buffer, uint32 buffer_len) {
  uint32 sector_index = offset / fs.disk.sector_size;
  uint32 count = 0;

  fs_node_t node;
  fs_resolve(path, &node);

  if (offset > node.entry.size && !(buffer_len % fs.disk.sector_size))
    return count;
  bezero(buffer, buffer_len);

  while (count < buffer_len) {
    uint32 lba = fs_map_sector(node.entry.index_lba, sector_index);
    if (!lba)
      return count;
    ata_read_sector(lba, buffer);
    count += fs.disk.sector_size;
    buffer += fs.disk.sector_size;
    sector_index++;
  }

  return count;
}

void fs_new_entry(int8 *path, int8 *name, uint8 type) {

  if (!fs.sb.root.index_lba)
    return;

  uint16 name_size = strlen(name) + 1;
  if (name_size > FS_NAME_MAX)
    return;

  fs_entry_t entry;
  bezero(&entry, sizeof(fs_entry_t));
  memcpy(name, entry.name, name_size);
  entry.type = type;
  fs_write(path, fs.sb.root.size, &entry, sizeof(fs_entry_t));
}

void fs_ls(int8 *path) {

  fs_node_t node;

  fs_resolve(path, &node);

  fs_entry_t *buffer = pmm_alloc(1);
  uint32 buff_len = PMM_FRAME_SIZE;

  uint32 entry_count = buff_len / sizeof(fs_entry_t);
  uint32 offset = 0;
  while (offset < node.entry.size) {
    fs_read("/", offset, buffer, PMM_FRAME_SIZE);
    for (uint32 i = 0; i < entry_count && offset < node.entry.size; i++) {
      kprintf("%s\n", buffer[i].name);
      offset += sizeof(fs_entry_t);
    }
  }
  pmm_free(buffer, 1);
  return;
}
