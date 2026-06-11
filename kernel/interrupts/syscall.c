#include "syscall.h"
#include "type.h"
#include <kprint.h>

void system_call(uint32 *esp) {
  kmemory_dump_hex(esp, 64);
  uint32 edi = *++esp;
  uint32 esi = *++esp;
  uint32 ebp = *++esp;
  uint32 ESP = *++esp;
  uint32 ebx = *++esp;
  uint32 edx = *++esp;
  uint32 ecx = *++esp;
  uint32 eax = *++esp;
  // kprint_dec(eax);
}
