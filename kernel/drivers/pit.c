#include "type.h"
#include <pic.h>
#include <pit.h>

uint64 ticks = 0;

uint64 pit_get_tick() { return ticks; }

void timer_handler() {
  ticks++;
  pic_send_eoi(0);
}
