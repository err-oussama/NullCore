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
  for (volatile int i = 0; i < 10; i++)
    ;
  syscall();
}
void tss_test() {
  uint32 user_stack = (uint32)pmm_alloc();
  get_tss()->esp0 = (uint32)pmm_alloc() + 0x1000;
  uint32 user_code = (uint32)user_procees_mock & 0xFFFFF000;
  if (!user_stack || !get_tss()->esp0)
    return;
  kprint_str("\nRing 0 -> Ring 3\n");
  mmu_map_page((uint32 *)read_cr3(), user_stack, user_stack,
               MMU_PTE_P | MMU_PTE_U_MODE | MMU_PTE_RW);
  mmu_map_page((uint32 *)read_cr3(), user_code, user_code,
               MMU_PTE_P | MMU_PTE_U_MODE | MMU_PTE_RW);
  kprint_str("Deascalting start\n");
  deascalate(user_procees_mock, user_stack + 0x1000);
  kprint_str("Deascalting end\n");
}
