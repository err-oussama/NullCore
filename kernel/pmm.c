#include "pmm.h"

static PMP pmp; // Physical Memory Pool

void pmm_init(uint32 pmp_start, uint32 pmp_size) {
  pmp.start = pmp_start;
  pmp.size = pmp_size;
}
uint32 pmm_get_pool_start() { return pmp.start; };
uint32 pmm_get_pool_size() { return pmp.size; };
