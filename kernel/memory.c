#include "memory.h"
#include "kernel.h"
#include "kprint.h"
#include "multiboot_metadata.h"
#include "pmm.h"
#include "type.h"

static heap_page *heap_memory = NULL; // the first page of the heap

void init_heap() {
  heap_memory = (void *)pmm_alloc();
  if (heap_memory) {
    memset(heap_memory, 0, 0x1000);
    heap_memory->free_space = 0x1000 - sizeof(heap_page);
    heap_memory->next = NULL;
    heap_memory->last_block = (heap_block *)&heap_memory[1];
  }
}

void *kmalloc(uint32 size) {
  if (!heap_memory)
    return NULL;
  heap_page *page = heap_memory;
  heap_block *block = (heap_block *)&page[1];
  size = (size + 3) & ~3;
  if (size < page->free_space) {
    while (block < page->last_block) {
      if (is_block_free(*block))
        return &block[1];
      block += sizeof(heap_block) + block_size(*block);
    }
    if (block + sizeof(heap_block) + size < page + 0x1000) {
      *block = size | 1;
      page->free_space -= size;
      page->last_block = block;
      return kmemory_dump_hex(page, 0x20), block + sizeof(heap_block);
    }
  }

  return NULL;
}
//  void kfree(ptr p);

uint32 block_size(heap_block block) { return block >> 1; }
uint8 is_block_free(heap_block block) { return !(block & 1); }

void memset(void *addr, uint32 c, uint32 size) {
  uint8 *p = (uint8 *)addr;
  while (size--) {
    p[size] = c;
  }
}

void show_physical_memory() {

  kprint("\n------------------KERNEL------------------");
  kprint("\nmultiboot header: 0x");
  kprint_hex((uint32)&multiboot_header);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_text - (uint32)&multiboot_header);

  kprint("\n   .text        : 0x");
  kprint_hex((uint32)&kernel_text);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_rodata - (uint32)&kernel_text);

  kprint("\n   .rodata      : 0x");
  kprint_hex((uint32)&kernel_rodata);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_data - (uint32)&kernel_rodata);

  kprint("\n   .data        : 0x");
  kprint_hex((uint32)&kernel_data);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_bss - (uint32)&kernel_data);

  kprint("\n   .bss         : 0x");
  kprint_hex((uint32)&kernel_bss);
  kprint(", size 0x");
  kprint_hex((uint32)&kernel_end - (uint32)&kernel_bss);

  kprint("\n   end of kernel: 0x");
  kprint_hex((uint32)&kernel_end);
  kprint("\n------------------------------------------\n");
}
