#include "vga_print.h"

void kclear_screen() { vga_clear_screen(); }
void kprint(char *str) { vga_print(str); }
void kprint_warn(char *str) { vga_print_warn(str); }
void kprint_err(char *str) { vga_print_err(str); }

void kprint_cha(uint8 c) { vga_print_cha(c); }
void kprint_dec(long n) { vga_print_dec(n); }
void kprint_hex(unsigned long n) { vga_print_hex(n); }
void kprint_bin(unsigned long n) { vga_print_bin(n); }

void kmemory_dump_bin(void *ptr, unsigned long size) {
  vga_memory_dump_bin(ptr, size);
}
void kmemory_dump_hex(void *ptr, unsigned long size) {
  vga_memory_dump_hex(ptr, size);
}
