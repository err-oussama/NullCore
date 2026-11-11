#ifndef FS_H
#define FS_H
#include <types.h>

#define FS_MAGIC "NULL-_-CORE_-_FS"
#define FS_MAGIC_LEN 0x10
#define FS_VERSION 0

#define FS_EXTENTS_MAX 63
#define FS_NAME_MAX 54

#define FS_ENTRY_TYPE_FILE 0x1
#define FS_ENTRY_TYPE_DIR 0x2

#define FS_PATH_SEP '/'
#define FS_ROOT_PATH "/"

typedef struct {
  uint32 sector_count;
  uint32 sector_size;
} fs_disk_t;

typedef struct __attribute__((packed)) {
  uint32 start;
  uint32 count;
} fs_extent_t;

typedef struct __attribute__((packed)) {
  uint32 next; // Next index sector LBA; 0 = last one.
  uint32 padding;
  fs_extent_t extents[FS_EXTENTS_MAX];
} fs_index_t;

typedef struct __attribute__((packed)) {
  uint32 index_lba;
  uint32 size;
  uint8 type;
  int8 name[FS_NAME_MAX + 1];
} fs_entry_t;

typedef struct {
  fs_entry_t entry;
  uint32 entry_lba;
  uint32 entry_offset;
} fs_node_t;

typedef struct __attribute__((packed)) {
  uint8 magic[FS_MAGIC_LEN];
  uint32 version;
  uint32 sector_count;

  uint32 sector_size;
  uint32 bitmap_start;
  uint32 bitmap_sectors;
  uint32 bitmap_bytes;
  uint32 data_start;
  fs_entry_t root;
} fs_superblock_t;

typedef struct {
  fs_disk_t disk;
  fs_superblock_t sb;
  uint8 *bitmap;
  uint32 bitmap_frames;
} fs_t;

_Static_assert(sizeof(fs_entry_t) == 64, "fs_entry_t must be 64 byte");
_Static_assert(sizeof(fs_extent_t) == 8, "fs_extent_t must be 8 byte");

void fs_init_disk();

void fs_new_entry(int8 *path, int8 *name, uint8 type);

uint32 fs_get_lba(fs_index_t *index, uint32 sector);
#endif
