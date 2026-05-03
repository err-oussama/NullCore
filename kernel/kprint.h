#ifndef PRINT_H
#define PRINT_H

#include "type.h"
#include "vga_print.h"

void kprint(char *str);
void kprint_cha(uint8 c);
void kprint_wrn(char *str);
void kprint_err(char *str);
void kprint_dec(long n);
void kprint_hex(unsigned long n);
void kprint_bin(unsigned long n);
void kprint_byt(uint8 byte);

void kmemory_dump_bin(void *ptr, unsigned long size);
void kmemory_dump_hex(void *ptr, unsigned long size);

#endif
