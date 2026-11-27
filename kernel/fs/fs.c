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
  kprintf("root dir     : %u\n", superblock->root.entry.index_lba);
  kprintf("root size    : %u\n", superblock->root.entry.size);
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
  superblock->root.entry.index_lba = superblock->bitmap_start + sec_needed;
  superblock->root.entry.size = 0;
  superblock->root.entry.name[0] = '/';
  superblock->root.entry.name[1] = 0;
  superblock->root.entry_lba = 0;
  superblock->root.entry_offset = (void *)&fs.sb.root - (void *)&fs.sb;
  superblock->data_start = superblock->root.entry.index_lba + 1;

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
  fs_bitmap_load();
}

uint32 fs_update_sector(fs_node_t *node, uint32 sector, uint16 offset,
                        void *data, uint16 len) {

  void *buff = pmm_alloc(1);
  if (!buff)
    return 1;

  ata_read_sector(sector, buff);
  memcpy(data, buff + offset, len);
  ata_write_sector(sector, buff);

  bezero(buff, PMM_FRAME_SIZE);
  ata_read_sector(node->entry_lba, buff);
  fs_node_t *disk_node = buff + node->entry_offset;
  disk_node->entry.size += len;

  ata_write_sector(node->entry_lba, buff);

  if (!node->entry_lba)
    memcpy(buff, &fs.sb, sizeof(fs.sb));

  memcpy(disk_node, node, sizeof(fs_node_t));
  pmm_free(buff, 1);

  LOG("address: %x\n", (sector * fs.disk.sector_size) + offset);
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
  index->extents[i].count = 5;
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

uint32 fs_read_h(uint32 index_lba, uint32 offset, void *buffer, uint32 len) {

  bezero(buffer, len);

  uint32 sector_index = offset / fs.disk.sector_size;
  uint32 count = 0;

  while (count < len) {
    uint32 lba = fs_map_sector(index_lba, sector_index);
    if (!lba)
      return count;
    ata_read_sector(lba, buffer);
    count += fs.disk.sector_size;
    buffer += fs.disk.sector_size;
    sector_index++;
  }
  return count;
}

int32 fs_resolve(int8 *path, fs_node_t *node) {
  if (path[0] != '/') {
    kprintf("path should start with '/'\n");
    return -1;
  }

  if (!memcmp(path, fs.sb.root.entry.name, 2)) {
    memcpy(&fs.sb.root, node, sizeof(fs_node_t));
    LOG("resolve: root node\n");
    return 0;
  }

  int8 name[FS_NAME_MAX];

  uint32 si = 1;
  uint32 ei = 1;

  fs_index_t *idx = pmm_alloc(1);
  if (!idx) {
    kprintf("pmm_alloc failed\n");
    return -1;
  }

  fs_entry_t *buff = pmm_alloc(1);
  if (!buff) {
    pmm_free(idx, 1);
    kprintf("pmm_alloc failed\n");
    return -1;
  }

  fs_entry_t entry;
  memcpy(&fs.sb.root.entry, &entry, sizeof(fs_entry_t));

  uint32 entry_count = PMM_FRAME_SIZE / sizeof(fs_entry_t);
  while (ei < strlen(path)) {
    while (path[ei] && path[ei] != '/')
      ei++;
    bezero(idx, PMM_FRAME_SIZE);
    bezero(name, FS_NAME_MAX);
    memcpy(path + si, name, ei - si);
    do {
      ata_read_sector(entry.index_lba, idx);
      for (uint32 i = 0; i < FS_EXTENTS_MAX; i++) {
        fs_extent_t *extent = &idx->extents[i];
        if (!extent->start)
          continue;
        uint32 start = extent->start;
        uint32 end = extent->start + extent->count;
        for (uint32 sec = start; sec < end; sec++) {
          ata_read_sector(sec, buff);
          uint32 offset = 0;
          for (uint32 v = 0; v < 8; v++) {
            if (buff[i].index_lba == 0)
              break;
            if (!memcmp(buff[i].name, name, ei - si)) {
              memcpy(&buff[i], &entry, sizeof(fs_entry_t));
              LOG("e: %i %s\n", ei, entry.name);
              if (ei < strlen(path))
                continue;

              memcpy(&buff[i], &node->entry, sizeof(fs_entry_t));
              node->entry_lba = sec;
              node->entry_offset = offset;
              pmm_free(idx, 1);
              pmm_free(buff, 1);
              return 0;
            }
            offset += sizeof(fs_entry_t);
          }
        }
      }
    } while (idx->next);

    ei++;
    si = ei;
    LOG("s: %u\n", si);
  }
  pmm_free(idx, 1);
  pmm_free(buff, 1);
  return -1;
}

void fs_write(fs_node_t *node, uint32 offset, void *data, uint32 len) {
  uint32 sector_index = offset / fs.disk.sector_size;
  uint32 sector_offset = offset % fs.disk.sector_size;
  uint32 lba = fs_map_sector(node->entry.index_lba, sector_index);
  if (!lba)
    return;
  fs_update_sector(node, lba, sector_offset, data, len);
}

uint32 fs_read(int8 *path, uint32 offset, void *buffer, uint32 len) {
  fs_node_t node;
  if (fs_resolve(path, &node))
    return 0;
  if (offset > node.entry.size && !(len % fs.disk.sector_size))
    return 0;
  return fs_read_h(node.entry.index_lba, offset, buffer, len);
}

void fs_init_entry(fs_entry_t *entry, char *name, uint8 type) {
  uint16 name_size = strlen(name) + 1;
  if (name_size > FS_NAME_MAX || strchr(name, '/'))
    return;
  fs_extent_t extent = {.count = 1};
  fs_alloc_extent(&extent);
  if (!extent.start) {
    kprintf("allocation failed\n");
    return;
  }
  fs_bitmap_flush();

  bezero(entry, sizeof(fs_entry_t));
  memcpy(name, entry->name, name_size);
  entry->type = type;
  entry->index_lba = extent.start;

  void *buff = pmm_alloc(1);
  if (!buff) {
    kprintf("pmm_alloc: failed ");
    return;
  }
  bezero(buff, PMM_FRAME_SIZE);
  ata_write_sector(entry->index_lba, buff);
  pmm_free(buff, 1);
}

void fs_new_entry(int8 *path, int8 *name, uint8 type) {

  if (!fs.sb.root.entry.index_lba)
    return;
  fs_node_t node;

  if (fs_resolve(path, &node))
    return;

  LOG("node name: %s\n", node.entry.name);
  LOG("node.entr_lba: %u\n", node.entry_lba);
  LOG("node.entry_offset: %u\n", node.entry_offset);
  LOG("address: %x\n", node.entry_lba * fs.disk.sector_size);
  fs_entry_t entry;

  fs_init_entry(&entry, name, type);
  if (!entry.index_lba)
    return;
  fs_write(&node, node.entry.size, &entry, sizeof(fs_entry_t));
}

void fs_ls(int8 *path) {
  fs_node_t node;

  if (fs_resolve(path, &node)) {
    kprintf("no path: %q\n", path);
    return;
  }

  fs_entry_t *buffer = pmm_alloc(1);
  uint32 buff_len = PMM_FRAME_SIZE;

  uint32 entry_count = buff_len / sizeof(fs_entry_t);
  uint32 offset = 0;
  while (offset < node.entry.size) {
    fs_read_h(node.entry.index_lba, offset, buffer, PMM_FRAME_SIZE);
    for (uint32 i = 0; i < entry_count && offset < node.entry.size; i++) {
      kprintf("name: '%s', lba: %u\n", buffer[i].name, buffer[i].index_lba);
      offset += sizeof(fs_entry_t);
    }
  }
  pmm_free(buffer, 1);
  return;
}
