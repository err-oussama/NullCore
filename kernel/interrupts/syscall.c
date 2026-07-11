#include "syscall.h"
#include <kprint.h>

void sys_nothing(registers *regs) { kprint_str("syscall coming soon\n"); }

void sys_read(registers *regs) { kprint_str("Read syscall coming soon\n"); }

void sys_write(registers *regs) { kprint_str("Write syscall coming soon\n"); }
syscall_handler syscall_table[] = {sys_read,    sys_write,   sys_nothing,
                                   sys_nothing, sys_nothing, sys_nothing,
                                   sys_nothing, sys_nothing, sys_nothing};

void syscall_dispatch(uint32 edi, uint32 esi, uint32 ebp, uint32 ESP,
                      uint32 ebx, uint32 edx, uint32 ecx, uint32 eax) {

  if (eax > 9)
    return;
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
