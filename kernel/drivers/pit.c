#include "types.h"
#include <kprint.h>
#include <pic.h>
#include <pit.h>
#include <pmio.h>
#include <registers.h>
#include <task.h>
#include <vmm.h>

#include <tss.h>
#define TIME_SLICE 1000

static uint64 ticks = 0;

uint64 pit_get_tick() { return ticks; }

void pit_init(uint32 frequency) {
  uint32 divisor32 = 1193182 / frequency;
  uint16 divisor = (uint16)divisor32;

  outb(PIT_COMMAND, PIT_MODE_ACCESS_LOHIBYTE | PIT_MODE_OP_MODE_2);

  outb(PIT_CHANNEL0, divisor & 0xFF);
  outb(PIT_CHANNEL0, (divisor >> 0x8) & 0xFF);
}

uint32 timer_handler(uint32 esp) {
  ticks++;
  pic_send_eoi(0);
  uint32 fall_back_esp = esp;

  task *current = current_task();
  if (ticks - current->start_tick > TIME_SLICE) {

    current->is_running = 0;
    current->esp = esp;

    task *next = next_task();
    next->is_running = 1;
    next->start_tick = ticks;

    change_esp0(next->kernel_stack + 0x1000);
    set_current(next->id);
    mmu_switch(next->pd);

    fall_back_esp = next->esp;
  }
  return fall_back_esp;
}
