#ifndef TASK_H
#define TASK_H

#include <type.h>

typedef struct task task;

struct task {
  uint32 id;
  uint32 esp;
  uint64 start_tick;
  uint8 is_running;
  uint8 is_dead;
  void *frame_buffer;
  uint32 *pd;
};

void task_init();

int create_task(void (*task)());

task *current_task();
task *next_task();
void set_current(uint32 id);

uint32 *get_kernel_vmm();
#endif
