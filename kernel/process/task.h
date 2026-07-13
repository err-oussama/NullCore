#ifndef TASK_H
#define TASK_H

#include <types.h>

typedef struct task task;

struct task {
  uint32 id;
  uint32 esp;
  uint64 start_tick;
  uint8 is_running;
  uint8 is_dead;
  void *user_stack;
  uint32 *pd;
  void *kernel_stack;
};

void task_init();

int create_task(void (*task)());
void clean_task(uint32 id);

int create_user_task(void (*task)(), void *pd);
task *current_task();
task *next_task();
void set_current(uint32 id);

uint32 *get_kernel_vmm();
#endif
