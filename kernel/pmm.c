#include "pmm.h"
#include "kprint.h"

static PMP pmp; // Physical Memory Pool
static uint8 *bitmap;

void pmm_init(uint32 pmp_start, uint32 pmp_size) {
  pmp.start = pmp_start;
  pmp.size = pmp_size;
  bitmap = (uint8 *)pmp_start;
  // memset(bitmap, 0, 0x1000);
  uint32 bitmap_size = (((pmp_size / 0x1000) + 0x7) / 0x8);
  *bitmap |= 1;
}
uint32 pmm_get_pool_start() { return pmp.start; };
uint32 pmm_get_pool_size() { return pmp.size; };

uint32 pmm_addre_to_frame(uint32 addre) { return (addre - pmp.start) / 0x1000; }
uint32 pmm_frame_to_addre(uint32 frame) { return (frame * 0x1000) + pmp.start; }
uint8 pmm_is_frame_free(uint32 frame) { return 0; };
