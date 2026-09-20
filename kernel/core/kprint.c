#include "types.h"
#include <kprint.h>
#include <vga_print.h>

void kclear_screen() { vga_clear_screen(); }
void kprint_str(char *str) { vga_print(str); }
void kprint_wrn(char *str) { vga_print_warn(str); }
void kprint_err(char *str) { vga_print_err(str); }

void kprint_cha(uint8 c) { vga_print_cha(c); }
void kprint_dec(int32 n) { vga_print_dec(n); }
void kprint_hex(uint32 n) { vga_print_hex(n); }
void kprint_hex_padded(uint32 n, uint32 min_width) {
  vga_print_hex_padded(n, min_width);
}
void kprint_bin(uint32 n) { vga_print_bin(n); }
void kprint_hex64(uint64 n) {
  vga_print_hex((uint32)(n >> 32));
  vga_print_hex((uint32)n);
}

void kmemory_dump_bin(void *ptr, uint32 size) {
  vga_memory_dump_bin(ptr, size);
}
void kmemory_dump_hex(void *ptr, uint32 size) {
  vga_memory_dump_hex(ptr, size);
}

void kprintf(char *format, ...) {
  char *ptr;

  ptr = (char *)&format;
  ptr += sizeof(char *);

  while (*format) {
    if (*format == '%') {
      format++;
      if (*format == 'd' || *format == 'i') {
        kprint_dec(*(int32 *)(ptr));
        ptr += sizeof(uint32);
      } else if (*format == 'u') {
        kprint_dec(*(uint32 *)(ptr));
        ptr += sizeof(uint32);
      } else if (*format == 'c') {
        kprint_cha(*(uint8 *)(ptr));
        ptr += sizeof(uint32);
      } else if (*format == 's') {
        kprint_str(*(char **)ptr);
        ptr += sizeof(char *);
      } else if (*format == 'x' || *format == 'X') {
        kprint_hex(*(uint32 *)(ptr));
        ptr += sizeof(uint32);
      } else if (*format == 'p') {
        kprint_str("0x");
        kprint_hex(*(uint32 *)(ptr));
        ptr += sizeof(void *);
      } else {
        break;
      }
    } else {
      kprint_cha(*format);
    }
    format++;
  }
}
