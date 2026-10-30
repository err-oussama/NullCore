#ifndef FS_H
#define FS_H
#include <types.h>

#define FS_MAX_EXTENT 63

#define FS_MAGIC "NULL-_-CORE_-_FS"
#define FS_MAGIC_LEN 0x10

#define FS_VERSION 0

typedef struct {
  uint32 sector_count;
  uint32 sector_size;
} fs_disk_t;

typedef struct __attribute__((packed)) {
  uint32 start;
  uint32 count;
} fs_extent_t;

typedef struct __attribute__((packed)) {
  uint32 size;
  uint32 extent_count;
  fs_extent_t extents[];
} fs_file_index_t;

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

void fs_init_disk();
void *fs_get_bitmap();
void fs_alloc_extent(fs_extent_t *extent);
void fs_free_extent(fs_extent_t *extent);

#endif
