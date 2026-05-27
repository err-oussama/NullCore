#include <keyboard.h>
#include <kprint.h>
#include <pic.h>
#include <pmio.h>

void keyboard_handler() {
  uint8 scancode = inb(0x60);
  if (!(scancode & 0x80))
    kprint_cha(scancode_map[scancode]);
  pic_send_eoi(0x1);
}
