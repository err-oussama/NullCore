#ifndef PIT_H
#define PIT_H
#include <type.h>
// Programmable Interval Timer

#define PIT_MODE_CHANNEL_0 0x00
#define PIT_MODE_CHANNEL_1 0x40
#define PIT_MODE_CHANNEL_2 0x80
#define PIT_MODE_CHANNEL_RBC 0xC0 // Read Back Command

#define PIT_MODE_ACCESS_LATCH 0x00    // latch count value
#define PIT_MODE_ACCESS_LOBYTE 0x10   // low byte only
#define PIT_MODE_ACCESS_HIBYTE 0x20   // high byte only
#define PIT_MODE_ACCESS_LOHIBYTE 0x30 // low byte then high byte

#define PIT_MODE_OP_MODE_0 0x00 // Interrupt on terminal count
#define PIT_MODE_OP_MODE_1 0x02 // hardware re-triggerable on shot
#define PIT_MODE_OP_MODE_2 0x04 // rate generator
#define PIT_MODE_OP_MODE_3 0x06 // square wave generator
#define PIT_MODE_OP_MODE_4 0x08 // software triggered strobe
#define PIT_MODE_OP_MODE_5 0x0A // hardware triggered strobe

#define PIT_MODE_NUMBER_FORMAT_BINARY 0x00
#define PIT_MODE_NUMBER_FORMAT_BCD 0x01 // Binary Coded Decimal

#define PIT_CHANNEL0 0x40 // Timer channel connected to IRQ0
#define PIT_CHANNEL1 0x41 // oboslete in modern system
#define PIT_CHANNEL2 0x42 // PC speaker

#define PIT_COMMAND 0x43

void timer_handler(uint32 esp);
uint64 pit_get_tick();

void pit_init(uint32 frequency);

#endif
