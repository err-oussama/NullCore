#ifndef TASK_H
#define TASK_H
#include <type.h>

typedef struct cpu_context cpu_context;

struct cpu_context {
  uint32 eax, ebx, ecx, edx;
  uint32 edi, esi;
  uint32 ebp, esp;
  uint32 eip;
  uint32 eflags;
  uint16 cs, ds, es, fs, gs, ss;
};

uint32 save_context(cpu_context *context);
void load_context(cpu_context *context);
void switch_context(cpu_context *current_context, cpu_context *new_context);

uint32 get_esp();

#endif
