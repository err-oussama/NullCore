#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>

typedef struct syscall_registers registers;

struct syscall_registers {
  uint32 edi;
  uint32 esi;
  uint32 ebp;
  uint32 ebx;
  uint32 edx;
  uint32 ecx;
  uint32 eax;
};

typedef void (*syscall_handler)(registers *regs);

void syscall_enter(uint32 syscall_enter_n, uint32 ebx, uint32 ecx, uint32 edx,
                   uint32 esi, uint32 edi, uint32 ebp);

void syscall_dispatch(uint32 edi, uint32 esi, uint32 edp, uint32 ESP,
                      uint32 ebx, uint32 edx, uint32 ecx, uint32 eax);

#endif
