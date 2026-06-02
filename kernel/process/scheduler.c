#include "scheduler.h"
#include <pit.h>

#include "task.h"
#include <kprint.h>

void p1() {
  while (1)
    kprintf("t1");
}
void p2() {
  while (1)
    kprintf("t2");
}

static task tasks[5];
void tasks_init() {
  task t1;
  t1.id = 1;
  t1.is_running = 0;
  t1.start_tick = 0;
  t1.context.eip = p1;
  task t2;
  t2.id = 2;
  t2.is_running = 0;
  t2.start_tick = 0;
  t2.context.eip = p2;

  tasks[0] = t1;
  tasks[1] = t2;
}

void schedule() {
  if (tasks[0].is_running && tasks[0].start_tick + 10 >= pit_get_tick()) {
  }
}
