#include "tss.h"
#include "type.h"
#include <control_registers.h>
#include <kprint.h>
#include <pmm.h>
#include <string.h>
#include <vmm.h>

tss_t tss;

void tss_init() {
  memset(&tss, 0, sizeof(tss_t));
  tss.ss0 = 0x10;
  tss.esp0 = 0x0;
  tss.iobp_offset = sizeof(tss_t);
}

tss_t *get_tss() { return &tss; }

extern void syscall();

__attribute__((section(".user_test"))) void user_procees_mock() {
  syscall();
  for (volatile int i = 4; i < 100000000; i++)
    i;
  syscall();
  while (1)
    ;
}

void tss_test() {
  uint32 *pd = mmu_create_address_space(); // 10D000
  uint32 user_stack = (uint32)pmm_alloc(); // 10E000
  get_tss()->esp0 = (uint32)pmm_alloc() + 0x1000;
  uint32 user_code = (uint32)user_procees_mock & 0xFFFFF000; // 0x102000
  if (!user_stack)
    return;
  mmu_map_page(pd, user_stack, user_stack,
               MMU_PTE_P | MMU_PTE_U_MODE | MMU_PTE_RW);
  //  mmu_map_page(pd, 0x100000, 0x100000, MMU_PTE_P | MMU_PTE_U_MODE |
  //  MMU_PTE_RW);
  mmu_map_page(pd, user_code, user_code,
               MMU_PTE_P | MMU_PTE_U_MODE | MMU_PTE_RW);
  mmu_switch(pd);
  kprint_str("Ring 0 -> Ring 3\n");
  deascalate(user_procees_mock, user_stack + 0x1000);
}
