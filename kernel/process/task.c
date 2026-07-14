#include "task.h"
#include "types.h"
#include <syscall.h>

#include <control_registers.h>
#include <kprint.h>
#include <kstring.h>
#include <pit.h>
#include <pmm.h>
#include <registers.h>
#include <vmm.h>

task tasks[10];
uint32 user_stacks[10];
uint32 kernel_stacks[10];

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
  memset(&kernel_stacks, 0, 10 * sizeof(uint32));
  memset(&user_stacks, 0, 10 * sizeof(uint32));
  tasks[0].id = 0;
  tasks[0].is_running = 1;
  tasks[0].is_dead = 0;
  tasks[0].start_tick = 0;
  tasks[0].esp = 0x0;
  tasks[0].pd = (uint32 *)read_cr3();
}

void clean_task(uint32 id) {

  pmm_free_frame(kernel_stacks[id]);
  pmm_free_frame(user_stacks[id]);
  kernel_stacks[id] = 0;
  user_stacks[id] = 0;
  tasks[id].is_dead = 1;
  enable_interrupt();
  while (1) {
    asm("hlt");
  }
}

int create_user_task(void (*task)(), void *pd) {
  disable_interrupt();
  uint32 id = id_vault++;

  tasks[id].id = id;
  tasks[id].is_running = 0;
  tasks[id].start_tick = 0;

  uint32 k_stack = pmm_alloc();
  uint32 u_stack = pmm_alloc();
  if (!k_stack || !u_stack)
    return 1;

  kernel_stacks[id] = k_stack;
  user_stacks[id] = u_stack;

  mmu_map_page((void *)read_cr3(), k_stack, k_stack, MMU_PTE_P | MMU_PTE_RW);
  tasks[id].pd = pd;

  mmu_map_page(tasks[id].pd, u_stack, u_stack,
               MMU_PTE_P | MMU_PTE_RW | MMU_PTE_U_MODE);
  mmu_map_page(tasks[id].pd, (uint32)syscall_enter, (uint32)syscall_enter,
               MMU_PTE_P | MMU_PTE_RW | MMU_PTE_U_MODE);

  tasks[id].user_stack = (void *)u_stack;
  tasks[id].kernel_stack = (void *)k_stack;
  uint32 *stack_frame = (uint32 *)(k_stack + 0x1000);
  uint32 *user_stack_frame = (uint32 *)(u_stack + 0x1000);

  *--user_stack_frame = 0x0;                   // DUMMY return address
  *--user_stack_frame = 0x0;                   // DUMMY return address
  *--user_stack_frame = 0x0;                   // DUMMY return address
  *--user_stack_frame = 0x0;                   // DUMMY return address
  *--user_stack_frame = 0x0;                   // DUMMY return address
  *--user_stack_frame = id;                    // DUMMY return address
  *--user_stack_frame = 0x2;                   // DUMMY return address
  *--user_stack_frame = 0x0;                   // DUMMY return address
  *--user_stack_frame = (uint32)syscall_enter; // EIP for clean_task
  // ----------------------------------------------------
  *--stack_frame = 0x23;                     // SS
  *--stack_frame = (uint32)user_stack_frame; // ESP // user_stack
  *--stack_frame = 0x202;                    // EFLAGS
  *--stack_frame = 0x1B;                     // CS
  *--stack_frame = (uint32)task;             // EIP for iret
  // ----------------------------------------------------
  *--stack_frame = 0x0; // EAX
  *--stack_frame = 0x0; // ECX
  *--stack_frame = 0x0; // EDX
  *--stack_frame = 0x0; // EBX
  *--stack_frame = 0x0; // ESP
  *--stack_frame = 0x0; // EBP
  *--stack_frame = 0x0; // ESI
  *--stack_frame = 0x0; // EDI
  tasks[id].esp = (uint32)stack_frame;
  enable_interrupt();
  return 0;
}

int create_task(void (*task)()) {
  disable_interrupt();
  uint32 id = id_vault++;

  tasks[id].id = id;
  tasks[id].is_running = 0;
  tasks[id].start_tick = 0;
  tasks[id].pd = tasks[0].pd;

  kernel_stacks[id] = pmm_alloc();
  if (kernel_stacks[id] == 0)
    return 1;
  uint32 *stack_frame = (uint32 *)(kernel_stacks[id] + 0x1000);

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
  return 0;
}
