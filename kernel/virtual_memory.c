#include "virtual_memory.h"

uint32 mmu_make_entry(uint32 frame_address, uint16 flags) {
  return (frame_address & 0xfffff000) | flags;
}
