#include "task.h"
#include "type.h"

#include <control_registers.h>
#include <kprint.h>
#include <pit.h>
#include <pmm.h>
#include <registers.h>
#include <string.h>

task tasks[10];
uint32 task_frame_buffer[10];

static uint32 id_vault = 1;

uint32 current_id = 0;

void set_current(uint32 id) { current_id = id; }

task *current_task() { return &tasks[current_id]; }

task *next_task() {
  uint32 next = current_id + 1;

  for (next; next < id_vault; next++) {
    if (!tasks[next].is_dead)
      break;
  }
  return next == id_vault ? &tasks[0] : &tasks[next];
}

uint32 *get_kernel_vmm() { return tasks[0].pd; }

void task_init() {
  memset(&tasks, 0, 10 * sizeof(task));
  memset(&task_frame_buffer, 0, 10 * sizeof(uint32));
  tasks[0].id = 0;
  tasks[0].is_running = 1;
  tasks[0].is_dead = 0;
  tasks[0].start_tick = 0;
  tasks[0].esp = 0x0;
  tasks[0].pd = (uint32 *)read_cr3();
}

void clean_task(uint32 id) {

  pmm_free_frame(task_frame_buffer[id]);
  tasks[id].is_dead = 1;
  while (1)
    ;
}

int create_task(void (*task)()) {
  disable_interrupt();
  uint32 id = id_vault++;

  tasks[id].id = id;
  tasks[id].is_running = 0;
  tasks[id].start_tick = 0;
  tasks[id].pd = tasks[0].pd;

  task_frame_buffer[id] = pmm_alloc();
  uint32 *stack_frame = (uint32 *)(task_frame_buffer[id] + 0x1000);

  *--stack_frame = id;                 // ID of the task
  *--stack_frame = 0x00;               // DUMMY return address
  *--stack_frame = (uint32)clean_task; // EIP for clean_task
  *--stack_frame = 0x206;              // EFLAGS
  *--stack_frame = 0x8;                // CS
  *--stack_frame = (uint32)task;       // EIP for iret
  *--stack_frame = 0x0;                // EAX
  *--stack_frame = 0x0;                // ECX
  *--stack_frame = 0x0;                // EDX
  *--stack_frame = 0x0;                // EBX
  *--stack_frame = 0x0;                // ESP
  *--stack_frame = 0x0;                // EBP
  *--stack_frame = 0x0;                // ESI
  *--stack_frame = 0x0;                // EDI
  tasks[id].esp = (uint32)stack_frame;
  enable_interrupt();
}
