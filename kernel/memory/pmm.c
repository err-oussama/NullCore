#include "types.h"
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>

static PMP pmp;       // Physical Memory Pool
static uint8 *bitmap; // 0 = free / 1 = used
static uint32 bitmap_size;

void pmm_init(void *pmp_start, uint32 pmp_size) {
  pmp.start = pmp_start;
  pmp.size = pmp_size;
  bitmap = (uint8 *)pmp_start;
  bitmap_size = (((pmp_size / 0x1000) + 0x7) / 0x8);
  memset(bitmap, 0, bitmap_size);
  pmm_use_frame(0);
}

void pmm_info() {
  kprint_str("-----------Physical Memory Pool-----------\n");
  kprint_str("  start: 0x");
  kprint_hex((uint32)pmp.start);
  kprint_str("\n  end:   0x");
  kprint_hex((uint32)pmp.start + pmp.size);
  kprint_str("\n  size:  0x");
  kprint_hex(pmp.size);
  kprint_str(" Byte, 0x");
  kprint_hex(pmp.size / 0x1000);
  kprint_str(" Frame, 0x");
  kprint_hex(pmp.size / 1024 / 1024);
  kprint_str(" MB\n");
  kmemory_dump_bin(bitmap, 0x10);
  kprint_str("\n------------------------------------------\n");
}

void *pmm_get_pool_start() { return pmp.start; }

uint32 pmm_get_pool_size() { return pmp.size; }

uint32 pmm_addre_to_frame(void *addre) { return (addre - pmp.start) / 0x1000; }

void *pmm_frame_to_addre(uint32 frame) { return (frame * 0x1000) + pmp.start; }

uint8 pmm_is_frame_free(uint32 frame) {

  return !((bitmap[frame / 8] >> (frame % 8)) & 1);
};
uint8 pmm_is_addre_free(void *addre) {
  return pmm_is_frame_free(pmm_addre_to_frame(addre));
}

void pmm_show_bitmap() { kmemory_dump_bin(bitmap, bitmap_size); }

void pmm_use_frame(uint32 frame_number) {
  uint32 frame_offset = frame_number / 8;
  bitmap[frame_offset] = bitmap[frame_offset] | (1 << (frame_number % 8));
}
void pmm_free_frame(uint32 frame_number) {
  uint32 frame_offset = frame_number / 8;
  bitmap[frame_offset] = bitmap[frame_offset] & ~(1 << (frame_number % 8));
}

void *pmm_alloc(uint32 n_frames) {
  uint32 count = 0;
  for (uint32 i = 0; i < pmp.size / 0x1000; i++) {
    if (pmm_is_frame_free(i))
      count++;
    else
      count = 0;

    if (count == n_frames) {
      for (uint32 j = 0; j < count; j++) {
        pmm_use_frame(i - j);
      }
      return pmm_frame_to_addre(i - count + 1);
    }
  }
  return NULL;
}

void pmm_free(void *addre, uint32 n_frames) {
  uint32 frame_n = pmm_addre_to_frame(addre);
  for (uint32 i = 0; i < n_frames; i++)
    pmm_free_frame(frame_n++);
}
