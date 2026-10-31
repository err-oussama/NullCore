#ifndef FS_H
#define FS_H
#include <types.h>

#define FS_MAGIC "NULL-_-CORE_-_FS"
#define FS_MAGIC_LEN 0x10
#define FS_VERSION 0

#define FS_EXTENTS_MAX 63
#define FS_NAME_MAX 58

#define FS_ENTRY_TYPE_FILE 0x1
#define FS_ENTRY_TYPE_DIR 0x2

#define FS_PATH_SEP '/'
#define FS_ROOT_PATH "/"

typedef struct {
  uint32 sector_count;
  uint32 sector_size;
} fs_disk_t;

typedef struct __attribute__((packed)) {
  uint8 magic[FS_MAGIC_LEN];
  uint32 version;
  uint32 sector_count;
  uint32 sector_size;
  uint32 bitmap_start;
  uint32 bitmap_bytes;
  uint32 root_dir_start;
  uint32 data_start;
} fs_superblock_t;

typedef struct __attribute__((packed)) {
  uint32 index_lba;
  uint8 type;
  int8 name[FS_NAME_MAX + 1];
} fs_entry_t;

typedef struct __attribute__((packed)) {
  uint32 start;
  uint32 count;
} fs_extent_t;

typedef struct __attribute__((packed)) {
  uint32 size;
  uint32 extent_count;
  fs_extent_t extents[];
} fs_index_t;

_Static_assert(sizeof(fs_entry_t) == 64, "fs_entry_t must be 64 byte");
_Static_assert(sizeof(fs_extent_t) == 8, "fs_extent_t must be 8 byte");
_Static_assert(sizeof(fs_index_t) == 8, "fs_index_t must be 8 byte");

void fs_init_disk();
void *fs_get_bitmap();
void fs_alloc_extent(fs_extent_t *extent);
void fs_free_extent(fs_extent_t *extent);

#endif
