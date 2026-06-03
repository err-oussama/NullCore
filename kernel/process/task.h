#ifndef TASK_H
#define TASK_H

#include <type.h>

typedef struct task task;

struct task {
  uint32 id;
  uint32 esp;
  uint64 start_tick;
  uint8 is_running;
};

#endif
