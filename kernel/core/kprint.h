#ifndef PRINT_H
#define PRINT_H

#include <types.h>
#include <vga_print.h>

void kclear_screen();
void kprintf(char *format, ...);
void kprint_str(char *str);
void kprint_cha(uint8 c);
void kprint_wrn(char *str);
void kprint_err(char *str);
void kprint_dec(int32 n);

void kprint_hex(uint32 n);
void kprint_hex_padded(uint32 n, uint32 min_width);
void kprint_dec64(uint64 n);
void kprint_hex64(uint64 n);
void kprint_bin(uint32 n);
void kprint_byt(uint8 byte);

void kmemory_dump_bin(void *ptr, uint32 size);
void kmemory_dump_hex(void *ptr, uint32 size);

#endif
