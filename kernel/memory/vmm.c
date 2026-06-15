#include "vmm.h"
#include "control_registers.h"
#include "pmm.h"
#include "type.h"
#include <string.h>
#include <task.h>

uint32 mmu_make_entry(uint32 frame_address, uint16 flags) {
  return (frame_address & 0xFFFFF000) | flags;
}

void mmu_kernel_setup() {
  uint32 *pd = (uint32 *)pmm_alloc();
  memset((void *)pd, 0, 0x1000);

  mmu_map_pt(pd, MMU_PDE_P | MMU_PDE_RW);

  write_cr3((uint32)pd);
  write_cr0(read_cr0() | 0x80000000);
}

void mmu_map_pt(uint32 *pd, uint16 flags) {
  uint32 *pt = (uint32 *)pmm_alloc();
  memset((void *)pt, 0, 0x1000);
  for (uint32 i = 0; i < 1024; i++) {
    pt[i] = mmu_make_entry(i << 12, MMU_PTE_P | MMU_PTE_RW);
  }
  *pd = mmu_make_entry((uint32)pt, flags);
}

uint32 *mmu_create_address_space() {
  uint32 *pd = (uint32 *)pmm_alloc();
  memcpy(pd, get_kernel_vmm(), 0x1000);
  return pd;
}

void mmu_destroy_address_space(uint32 *pd) {
  for (uint32 i = 0; i < 0x400; i++) {
    if (pd[i] & MMU_PDE_P && pd[i] & MMU_PDE_U_MODE)
      pmm_free_frame(pd[i] >> 12);
  }
  pmm_free_frame((uint32)pd >> 12);
}
