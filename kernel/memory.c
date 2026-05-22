#include "memory.h"
#include "kernel.h"
#include "kprint.h"
#include "multiboot_metadata.h"
#include "pmm.h"
#include "type.h"

static heap_page *heap_memory = NULL; // the first page of the heap

void memory_print_page(uint32 id, uint32 part) {

  heap_page *page = heap_memory;
  for (uint32 i = 0; i < id; i++)
    page = page->next;

  kmemory_dump_hex(page + (part * 16), 16);
}
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
      page->free_space -= size | 1;
      page->last_block = block;
      return block + sizeof(heap_block);
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

  kprint_str("\n------------------KERNEL------------------");
  kprint_str("\nmultiboot header: 0x");
  kprint_hex((uint32)&multiboot_header);
  kprint_str(", size 0x");
  kprint_hex((uint32)&kernel_text - (uint32)&multiboot_header);

  kprint_str("\n   .text        : 0x");
  kprint_hex((uint32)&kernel_text);
  kprint_str(", size 0x");
  kprint_hex((uint32)&kernel_rodata - (uint32)&kernel_text);

  kprint_str("\n   .rodata      : 0x");
  kprint_hex((uint32)&kernel_rodata);
  kprint_str(", size 0x");
  kprint_hex((uint32)&kernel_data - (uint32)&kernel_rodata);

  kprint_str("\n   .data        : 0x");
  kprint_hex((uint32)&kernel_data);
  kprint_str(", size 0x");
  kprint_hex((uint32)&kernel_bss - (uint32)&kernel_data);

  kprint_str("\n   .bss         : 0x");
  kprint_hex((uint32)&kernel_bss);
  kprint_str(", size 0x");
  kprint_hex((uint32)&kernel_end - (uint32)&kernel_bss);

  kprint_str("\n   end of kernel: 0x");
  kprint_hex((uint32)&kernel_end);
  kprint_str("\n------------------------------------------\n");
}
