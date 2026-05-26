#ifndef PIC_H
#define PIC_H

#include "pmio.h"

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21

#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

#define PIC_EOI 0x20 // after every handler

void pic_init();
void pic_send_eoi(uint8 irq);

#endif
