#ifndef FS_H
#define FS_H
#include <types.h>

typedef struct {
  uint32 sector_count;
  uint32 sector_size;
} blk_dev_t;

#endif
