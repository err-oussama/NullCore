#include "task.h"
#include "type.h"

#include <kprint.h>
#include <pmm.h>
#include <string.h>

task demo_task;
uint32 task_frame_buffer[10];

uint32 id_vault = 0;

void task_init() {
  memset(&demo_task, 0, sizeof(task));
  memset(task_frame_buffer, 0, 10 * sizeof(uint32));
  kprintf("init_task\n");
}

task *get_task() { return &demo_task; }

int create_task(void (*task)()) {
  kprintf("create_task\n");

  uint32 id = 1;
  demo_task.id = id;
  demo_task.is_running = 1;
  task_frame_buffer[id] = pmm_alloc();
  uint32 *stack_frame = (uint32 *)(task_frame_buffer[id] + 0x1000);
  kprintf("frame-> '%p'\nstack frame-> '%p'\n", task_frame_buffer[id],
          stack_frame);

  memset((void *)(task_frame_buffer[0] + 0xFBF), 0, 64);
  *--stack_frame = 0x206;
  *--stack_frame = 0x8;
  *--stack_frame = (uint32)task;
  demo_task.esp = (uint32)stack_frame;
  kprintf("esp-> '%p'\n\n", demo_task.esp);
  kmemory_dump_hex(stack_frame, 16);
}
