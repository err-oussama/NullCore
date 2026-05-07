#include "vga_print.h"
#include "kernel.h"

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
int g_row = 0;
int g_col = 0;
volatile uint16 *vga = (uint16 *)0xB8000;

void vga_clear_screen() {
  for (uint32 i = 0; i < 25 * (80); i++)
    vga[i] = ((VGA_BLACK << 4 | VGA_BLACK) << 8) | ' ';
}
void vg_put_char(char c, int fg, int bg, int row, int col) {
  vga[row * 80 + col] = ((bg << 4 | fg) << 8) | c;
}

void vga_print_str(char *str, int fg, int bg) {
  while (*str) {
    if (g_row >= 25)
      break;
    if (*str == '\n' || g_col >= 80) {
      g_col = 0;
      g_row++;
      if (*str == '\n') {
        str++;
        continue;
      }
    }
    vga[g_row * 80 + g_col] = ((bg << 4 | fg) << 8) | *str++;
    g_col++;
  }
}

void vga_print_cha(uint8 c) {
  if (g_col >= 80) {
    g_col = 0;
    g_row++;
  }
  if (g_row >= 25)
    g_row = 0;
  if (c == '\b') {
    if (g_col == 0 && g_row != 0) {
      g_col = 79;
      if (g_row > 0)
        g_row--;
    }
    if (g_col || g_row)
      g_col--;
    vga[g_row * 80 + g_col] = ((VGA_BLACK << 4 | VGA_BLACK) << 8) | ' ';
  } else {
    vga[g_row * 80 + g_col] = ((VGA_BLACK << 4 | VGA_WHITE) << 8) | c;
    g_col++;
  }
}
void vga_print_err(char *str) { vga_print_str(str, VGA_BLACK, VGA_RED); }
void vga_print_warn(char *str) { vga_print_str(str, VGA_YELLOW, VGA_BLACK); }

void vga_print(char *str) { vga_print_str(str, VGA_WHITE, VGA_BLACK); }

void vga_print_dec(long n) {
  if (n < 0) {
    vga_print("-");
    n = -n;
  }
  vga_print_base(n, "0123456789", 10);
}

void vga_print_hex(uint32 n) { vga_print_base(n, "0123456789ABCDEF", 16); }
void vga_print_base(uint32 n, char *base, uint32 base_len) {
  char buff[40];
  int i = 39;

  buff[i] = '\0';
  if (n == 0)
    return (vga_print_str("0", VGA_WHITE, VGA_BLACK));
  while (n > 0) {
    buff[--i] = base[(n % base_len)];
    n /= base_len;
  }
  vga_print_str(&buff[i], VGA_WHITE, VGA_BLACK);
}

void vga_print_bin(uint32 n) { vga_print_base(n, "01", 2); }

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
  uint32 i = -1;
  uint8 *p = (uint8 *)ptr;

  while (++i < size) {
    if (!(i % 16)) {
      if (i)
        vga_print("\n");
      vga_print("0x");
      vga_print_hex((uint32)p + i);
      vga_print(": ");
    }
    vga_print(" ");
    vga_print_hex((uint32)(p[i] >> 0x4));
    vga_print_hex((uint32)(p[i] & 0x0f));
  }
}
