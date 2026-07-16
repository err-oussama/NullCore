#include "syscall.h"
#include "types.h"
#include <kprint.h>
#include <task.h>

void sys_nothing(registers *regs) { kprint_str("syscall coming soon\n"); }

void sys_read(registers *regs) { kprint_str("Read syscall coming soon\n"); }

void sys_write(registers *regs) {
  /* kprint_hex(regs->ebx); */
  kprint_cha(';');
  kprint_cha('\n');

  // ff
}

void sys_exit(registers *regs) { clean_task(regs->ebx); }

syscall_handler syscall_table[] = {sys_read,    sys_write,   sys_exit,
                                   sys_nothing, sys_nothing, sys_nothing,
                                   sys_nothing, sys_nothing, sys_nothing};

void syscall_dispatch(uint32 edi, uint32 esi, uint32 ebp, uint32 ESP,
                      uint32 ebx, uint32 edx, uint32 ecx, uint32 eax) {

  if (eax > 9) {
    kprintf("Syscall %p Not exist\n", eax);
    return;
  }
  registers regs;
  regs.eax = eax;
  regs.ecx = ecx;
  regs.edx = edx;
  regs.ebx = ebx;
  regs.ebp = ebp;
  regs.esi = esi;
  regs.edi = edi;
  syscall_table[eax](&regs);
}
