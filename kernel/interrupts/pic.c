#include "pic.h"
#include "type.h"

void pic_init() {
  outb(PIC_MASTER_COMMAND, 0x11);
  outb(PIC_MASTER_DATA, 0x20);
  outb(PIC_MASTER_DATA, 0x04);
  outb(PIC_MASTER_DATA, 0x01);
  outb(PIC_MASTER_DATA, 0x00);

  outb(PIC_SLAVE_COMMAND, 0x11);
  outb(PIC_SLAVE_DATA, 0x28);
  outb(PIC_SLAVE_DATA, 0x02);
  outb(PIC_SLAVE_DATA, 0x01);
  outb(PIC_SLAVE_DATA, 0x00);
}

void pic_send_eoi(uint8 irq) {
  if (irq >= 8)
    outb(PIC_SLAVE_COMMAND, PIC_EOI);
  outb(PIC_MASTER_COMMAND, PIC_EOI);
}
