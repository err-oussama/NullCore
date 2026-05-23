#ifndef MEMORY_H
#define MEMORY_H

#include "multiboot_metadata.h"
#include "type.h"

// bits 31-1 used as size of the block always 4 byte alignement
// bit 0 used as flage for free and used state: 0=free, 1=used
typedef uint32 heap_block;

typedef struct heap_page heap_page;
struct heap_page {
  heap_page *next;   // pointer to the next page
  uint32 free_space; // 0x1000 - used address
  heap_block *last_block;
};

void *kmalloc(uint32 size);
void kfree(void *p);

void memory_print_page(uint32 id, uint32 part);
void init_heap();
uint32 block_size(heap_block block);
uint8 is_block_free(heap_block block);
void show_physical_memory();
void memset(void *addr, uint32 c, uint32 size);

void *get_heap_start();
#endif
