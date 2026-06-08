#include "task.h"
#include "type.h"

#include <kprint.h>
#include <pit.h>
#include <pmm.h>
#include <registers.h>
#include <string.h>

task tasks[2];
uint32 task_frame_buffer[10];

uint32 id_vault = 0;

task *get_task() { return tasks; }

void task_init() {
  memset(&tasks, 0, 10 * sizeof(task));
  memset(task_frame_buffer, 0, 10 * sizeof(uint32));
}

int create_task(void (*task)()) {
  disable_interrupt();
  uint32 id = id_vault++;
  tasks[id].id = id;

  tasks[id].is_running = 0;
  tasks[id].start_tick = pit_get_tick() + 10 * id;
  task_frame_buffer[id] = pmm_alloc();
  uint32 *stack_frame = (uint32 *)(task_frame_buffer[id] + 0x1000);
  *--stack_frame = 0x206;        // EFLAGS
  *--stack_frame = 0x8;          // CS
  *--stack_frame = (uint32)task; // EIP for iret
  *--stack_frame = 0x0;          // EAX
  *--stack_frame = 0x0;          // ECX
  *--stack_frame = 0x0;          // EDX
  *--stack_frame = 0x0;          // EBX
  *--stack_frame = 0x0;          // ESP
  *--stack_frame = 0x0;          // EBP
  *--stack_frame = 0x0;          // ESI
  *--stack_frame = 0x0;          // EDI
  tasks[id].esp = (uint32)stack_frame;
  enable_interrupt();
}
