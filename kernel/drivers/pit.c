#include "pmio.h"
#include "type.h"
#include <kprint.h>
#include <pic.h>
#include <pit.h>
#include <task.h>

#include <registers.h>

static uint64 ticks = 0;

uint64 pit_get_tick() { return ticks; }

void pit_init(uint32 frequency) {
  uint32 divisor32 = 1193182 / frequency;
  uint16 divisor = (uint16)divisor32;

  outb(PIT_COMMAND, PIT_MODE_ACCESS_LOHIBYTE | PIT_MODE_OP_MODE_2);

  outb(PIT_CHANNEL0, divisor & 0xFF);
  outb(PIT_CHANNEL0, (divisor >> 0x8) & 0xFF);
}

void timer_handler(uint32 esp) {
  ticks++;
  pic_send_eoi(0);
  task *tasks = get_task();

  if (!tasks[0].is_running && tasks[0].start_tick <= ticks) {
    tasks[0].is_running = 1;
    tasks[1].is_running = 0;
    tasks[0].start_tick = ticks + 1000;
    tasks[1].esp = esp;
    kprint_str("switch to 1\n");
    switch_esp(tasks[0].esp);
  }

  if (!tasks[1].is_running && tasks[1].start_tick <= ticks) {
    tasks[1].is_running = 1;
    tasks[0].is_running = 0;
    tasks[1].start_tick = ticks + 1000;
    tasks[0].esp = esp;
    kprint_str("switch to 2\n");
    switch_esp(tasks[1].esp);
  }
}
