#include "pmio.h"
#include "type.h"
#include <kprint.h>
#include <pic.h>
#include <pit.h>
#include <task.h>

#include <registers.h>

static uint64 ticks = 0;

void timer_handler() {
  ticks++;
  pic_send_eoi(0);
  task *demo_task = get_task();
  if (demo_task->id == 1 && demo_task->esp == 0x10CFF4) {
    demo_task->id = 0;
    switch_esp(demo_task->esp);
  }
}

uint64 pit_get_tick() { return ticks; }

void pit_init(uint32 frequency) {
  uint32 divisor32 = 1193182 / frequency;
  uint16 divisor = (uint16)divisor32;

  outb(PIT_COMMAND, PIT_MODE_ACCESS_LOHIBYTE | PIT_MODE_OP_MODE_2);

  outb(PIT_CHANNEL0, divisor & 0xFF);
  outb(PIT_CHANNEL0, (divisor >> 0x8) & 0xFF);
}
