#include "tss.h"
#include <string.h>

tss_t tss;

void tss_init() {
  memset(&tss, 0, sizeof(tss_t));
  tss.ss0 = 0x10;
  tss.esp0 = 0;
  tss.iobp_offset = sizeof(tss_t);
}
tss_t *get_tss() { return &tss; }
