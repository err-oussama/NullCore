#include "pmm.h"

static PMP pmp;       // Physical Memory Pool
static uint8 *bitmap; // 0 = free / 1 = used

void pmm_init(uint32 pmp_start, uint32 pmp_size) {
  pmp.start = pmp_start;
  pmp.size = pmp_size;
  bitmap = (uint8 *)pmp_start;
  uint32 bitmap_size = (((pmp_size / 0x1000) + 0x7) / 0x8);
  memset(bitmap, 0, bitmap_size);
  pmm_use_frame(0);
}

uint32 pmm_get_pool_start() { return pmp.start; }
uint32 pmm_get_pool_size() { return pmp.size; }

uint32 pmm_addre_to_frame(uint32 addre) { return (addre - pmp.start) / 0x1000; }

uint32 pmm_frame_to_addre(uint32 frame) { return (frame * 0x1000) + pmp.start; }

uint8 pmm_is_frame_free(uint32 frame) {
  return !((bitmap[frame / 8] >> (frame % 8)) & 1);
};
uint8 pmm_is_addre_free(uint32 addre) {
  return pmm_is_frame_free(pmm_addre_to_frame(addre));
}

void pmm_use_frame(uint32 frame) {
  uint32 frame_offset = frame / 8;
  bitmap[frame_offset] = bitmap[frame_offset] | (1 << (frame % 8));
}
void pmm_free_frame(uint32 frame) {
  uint32 frame_offset = frame / 8;
  bitmap[frame_offset] = bitmap[frame_offset] & ~(1 << (frame % 8));
}

uint32 pmm_alloc() {
  for (uint32 i = 0; i < pmp.size / 0x1000; i++) {
    if (pmm_is_frame_free(i))
      return (pmm_use_frame(i), pmm_frame_to_addre(i));
  }
  return 0;
}

void pmm_free(uint32 addre) { pmm_free_frame(pmm_addre_to_frame(addre)); }
