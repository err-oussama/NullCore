#ifndef FS_H
#define FS_H
#include <types.h>

#define FS_MAX_EXTENT 63

typedef struct {
  uint32 sector_count;
  uint32 sector_size;
} fs_disk_t;

typedef struct {
  uint32 start;
  uint32 count;
} fs_extent_t;

typedef struct {
  uint32 size;
  uint32 extent_count;
  fs_extent_t extents[];
} fs_file_index_t;

void fs_init_disk();
void *fs_get_bitmap();

#endif
