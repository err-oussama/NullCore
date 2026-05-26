#ifndef VGA_PRINT_H
#define VGA_PRINT_H

#include "type.h"

#define VGA_BLACK 0x0
#define VGA_BLUE 0x1
#define VGA_GREEN 0x2
#define VGA_CYAN 0x3
#define VGA_RED 0x4
#define VGA_MAGENTA 0x5
#define VGA_BROWN 0x6
#define VGA_LIGHT_GRAY 0x7
#define VGA_DARK_GRAY 0x8
#define VGA_LIGHT_BLUE 0x9
#define VGA_LIGHT_GREEN 0xA
#define VGA_LIGHT_CYAN 0xB
#define VGA_LIGHT_RED 0xC
#define VGA_LIGHT_MAGENTA 0xD
#define VGA_YELLOW 0xE
#define VGA_WHITE 0xF

void vga_clear_screen();
void vga_print_base(uint32 n, char *base, uint32 base_len);
void vga_print_str(char *str, int fg, int bg);

void vga_print_cha(uint8 c);
void vga_print_dec(long n);
void vga_print_hex(uint32 n);
void vga_print(char *str);
void vga_print_err(char *str);
void vga_print_warn(char *str);
void vga_print_bin(uint32 n);
void vga_print_byte(uint8 byte);

void vga_memory_dump_bin(void *ptr, uint32 size);
void vga_memory_dump_hex(void *ptr, uint32 size);

#endif
