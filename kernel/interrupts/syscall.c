#include "syscall.h"
#include "type.h"
#include <kprint.h>

void syscall_not_implemented(registers *regs) {
  kprint_str("syscall coming soon\n");
}

syscall_handler syscall_table[] = {
    syscall_not_implemented, syscall_not_implemented, syscall_not_implemented,
    syscall_not_implemented, syscall_not_implemented, syscall_not_implemented,
    syscall_not_implemented, syscall_not_implemented, syscall_not_implemented};

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
