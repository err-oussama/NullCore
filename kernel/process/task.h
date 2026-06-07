#ifndef TASK_H
#define TASK_H

#include <type.h>

typedef struct task task;

struct task {
  uint32 id;
  uint32 esp;
  uint64 start_tick;
  uint8 is_running;
  void *frame_buffer;
};

void task_init();

int create_task(void (*task)());

task *get_task();

#endif
