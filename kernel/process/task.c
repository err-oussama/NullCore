#include "task.h"
#include "type.h"

#include <pmm.h>
#include <string.h>

task queue[10];
uint32 task_frame_buffer[10];

uint32 id_vault = 0;

void task_init() {
  memset(queue, 0, 10 * 4);
  memset(task_frame_buffer, 0, 10 * 4);
}

task *get_task_queue() { return queue; }

int create_task(void (*task)()) {

  uint32 id = id_vault++;
  queue[id].id = id;
  task_frame_buffer[id] = pmm_alloc();
  uint32 *stack_frame = (uint32 *)(task_frame_buffer[id] + 0xFFC);
  if (task_frame_buffer[id])
    return 1;

  memset((void *)(task_frame_buffer[0] + 0xFBF), 0, 64);
  *--stack_frame = 0x202;
  *--stack_frame = 0x8;
  *--stack_frame = (uint32)task;
  queue[id].esp = (uint32)(stack_frame - 28);
}
