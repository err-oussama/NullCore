#include "vga_print.h"
#include "types.h"

/*

Video Graphics Array (VGA) -- Text Mode Memory layout
        in text mode, VGA maps a region of memory directly to the screen.
        You can think of it as a 2D grid(typically 80 columns x 25 rows),
        where each cell on screen corresponds to exactly 2 Byte in memory
        - Byte 1: Attribute byte: Controls color.
                >The upper 4 bits set the backgroud color
                >The lower 4 bits set the foregroud color
        - Byte 2: Charachter byte: an ASCII(or code page 437) value for
                        the charachter to display in the cell


*/

uint32 vga_offset = 0;

volatile uint16 *vga = (uint16 *)0xB8000;

void vga_clear_screen() {
  for (uint32 i = 0; i < VGA_MAX_ROW * (VGA_MAX_COL); i++)
    vga[i] = ((VGA_BLACK << 4 | VGA_BLACK) << 8) | ' ';
}
void vga_put_char(uint8 c, int fg, int bg) {

  switch (c) {

  case '\b':
    vga[vga_offset] = ((bg << 4 | fg) << 8) | ' ';
    vga_offset--;
    break;

  case '\n':
    vga_offset += VGA_MAX_COL - (vga_offset % VGA_MAX_COL);
    break;

  default:
    vga[vga_offset] = ((bg << 4 | fg) << 8) | c;
    vga_offset++;
  }

  vga_offset = vga_offset >= VGA_MAX_COL * VGA_MAX_ROW ? 0 : vga_offset;
}
void vga_print_str(char *str, int fg, int bg) {
  while (*str)
    vga_put_char(*str++, fg, bg);
}

void vga_print_cha(uint8 c) { vga_put_char(c, VGA_WHITE, VGA_BLACK); }

void vga_print_base(uint32 n, char *base, uint32 base_len, uint32 min_width) {

  uint32 buff_len = 20;
  char buff[buff_len];
  int i = buff_len;

  for (uint32 j = 0; j <= i; j++)
    buff[j] = '0';

  if (n == 0) {
    for (uint32 i = 1; i < min_width; i++)
      vga_print_cha('0');
    vga_print_cha('0');
    return;
  }
  while (n > 0) {
    buff[--i] = base[(n % base_len)];
    n /= base_len;
  }

  if (min_width)
    i = buff_len - min_width;

  while (i < buff_len)
    vga_print_cha(buff[i++]);
}

void vga_print_err(char *str) { vga_print_str(str, VGA_BLACK, VGA_RED); }
void vga_print_warn(char *str) { vga_print_str(str, VGA_YELLOW, VGA_BLACK); }

void vga_print(char *str) { vga_print_str(str, VGA_WHITE, VGA_BLACK); }

void vga_print_int(int32 n) {
  if (n < 0) {
    vga_print("-");
    n = -n;
  }
  vga_print_base(n, "0123456789", 10, 0);
}
void vga_print_uint(uint32 n) { vga_print_base(n, "0123456789", 10, 0); }
void vga_print_hex(uint32 n) { vga_print_base(n, "0123456789ABCDEF", 16, 0); }
void vga_print_hex_padded(uint32 n, uint32 len) {
  vga_print_base(n, "0123456789ABCDEF", 16, len);
}

void vga_print_bin(uint32 n) { vga_print_base(n, "01", 2, 8); }

void vga_print_byte(uint8 byte) {
  uint32 i = 8;
  while (i--)
    vga_print((byte >> i) & 1 ? "1" : "0");
}

void vga_memory_dump_bin(void *ptr, uint32 size) {
  uint32 i = -1;
  uint8 *p = (uint8 *)ptr;

  while (++i < size) {
    if (!(i % 4)) {
      if (i)
        vga_print("\n");
      vga_print("0x");
      vga_print_hex((uint32)(p + i));
      vga_print(": ");
    }
    vga_print(" ");
    vga_print_byte(p[i]);
  }
}

void vga_memory_dump_hex(void *ptr, uint32 size) {
  uint8 *p = (uint8 *)ptr;

  for (uint32 i = 0; i < size; i++) {
    if (!(i % 16)) {
      if (i)
        vga_print("\n");
      vga_print("0x");
      vga_print_hex((uint32)p + i);
      vga_print(": ");
    }
    if (!(i % 2) && i != 0 && i != 0 && (i % 16))
      vga_print(" ");

    vga_print_hex((uint32)(p[i] >> 0x4));
    vga_print_hex((uint32)(p[i] & 0x0F));
  }
}
