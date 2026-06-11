#ifndef SYSCALL_H
#define SYSCALL_H

#include <type.h>

void system_call(uint32 *esp);

void make_syscall();

void syscall(uint32 syscall_n, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi,
             uint32 edi, uint32 ebp);

#endif
