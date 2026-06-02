#ifndef TASK_H
#define TASK_H

#include <context.h>

#include <type.h>
typedef struct task task;

struct task {
  uint64 start_tick;
  uint32 id;
  uint8 is_running;
  cpu_context context;
};

#endif
