#ifndef MEMORY_H
#define MEMORY_H

#include "multiboot_metadata.h"
#include "type.h"

/*
logic of allocation will follow this rule:

- global ptr to the head of heap_page so i can walk through pages

- first check if the size of allocation is bigger then a size page
- if true:
                allocate more then frame
                keep just one heap_page for the first page
                the size will tell how much frame will be needed to be freed
if false:
        check if page can pass a heap_block
        if true:
                return the address of new heap_block
        else:
                allocate new page then return the heap_block from it

*/

typedef struct heap_page heap_page;

// bits 31-1 used as size of the block always 4 byte alignement
// bit 0 used as flage for free and used state: 0=free, 1=used
typedef uint32 heap_block;

struct heap_page {
  heap_page *next;   // pointer to the next page
  uint32 free_space; // 0x1000 - used address
  heap_block *last_block;
};

void *kmalloc(uint32 size);
void kfree(void *p);

void init_heap();
uint32 block_size(heap_block block);
uint8 is_block_free(heap_block block);
void show_physical_memory();
void memset(void *addr, uint32 c, uint32 size);

#endif
