#ifndef FS_H
#define FS_H
#include <types.h>

#define FS_MAX_EXTENT 63

typedef struct {
  uint32 sector_count;
  uint32 sector_size;
} disk_t;

typedef struct {
  uint32 start;
  uint32 count;
} extent_t;

typedef struct {
  uint32 size;
  uint32 extent_count;
  extent_t extents[];
} file_index_t;

#endif
