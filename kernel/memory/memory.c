#include "memory.h"
#include <kernel.h>
#include <kprint.h>
#include <multiboot_metadata.h>
#include <pmm.h>
#include <string.h>
#include <type.h>

static heap_page *heap_memory = NULL; // the first page of the heap

heap_page *get_heap_start() { return heap_memory; }

void init_heap() {
  heap_memory = (void *)pmm_alloc();
  if (heap_memory) {
    memset(heap_memory, 0, 0x1000);
    heap_memory->free_space = 0x1000 - sizeof(heap_page);
    heap_memory->next = NULL;
    heap_memory->last_block = NULL;
  }
}

int add_page() {
  heap_page *current = heap_memory;

  while (current->next)
    current = current->next;

  heap_page *new_page = (heap_page *)pmm_alloc();
  if (new_page) {
    current->next = new_page;
    new_page->free_space = PAGE_SIZE - sizeof(heap_page) - sizeof(heap_block);
    new_page->next = NULL;
    new_page->last_block = NULL;
    return 0;
  }
  return 1;
}

void *get_block_from_page(heap_page *page, uint32 size) {
  heap_block *block = page->last_block;
  if (!block) {
    block = (heap_block *)&page[1];
    *block = size | 1;
    page->free_space -= size + sizeof(heap_block);
    page->last_block = block;
    return &block[1];
  }
  uint8 *ptr = NULL;
  while (block <= page->last_block) {
    if (is_block_free(*block) && block_size(*block) >= size)
      return &block[1];
    ptr = (uint8 *)block;
    ptr += block_size(*block) + sizeof(heap_block);
    block = (heap_block *)ptr;
  }
  ptr = (uint8 *)block;
  if (ptr + sizeof(heap_block) + size > page + PAGE_SIZE)
    return NULL;

  *block = size | 1;
  page->free_space -= size + sizeof(heap_block);
  page->last_block = block;
  return &block[1];
}
void *kmalloc(uint32 size) {

  if (!heap_memory || !size)
    return NULL;

  heap_page *page = heap_memory;
  size = (size + 3) & ~3;

  /* if (size > PAGE_SIZE) will be supported when needed
                   return pages
   */

  while (page->next && page->free_space < size)
    page = page->next;
  if (!page->next && page->free_space < size) {
    add_page();
    page = page->next;
    if (!page)
      return NULL;
  }
  return get_block_from_page(page, size);
}

void kfree(void *ptr) {
  uint8 *p = ptr;
  heap_page *page;
  uint32 page_start = (uint32)ptr;
  heap_block *block = (heap_block *)(p - sizeof(heap_block));
  *block &= ~0x3;
  page_start &= ~0xFFF;
  kprintf("page address is : %p\n", page_start);
  page = (heap_page *)page_start;
  page->free_space += block_size(*block);
}

uint32 block_size(heap_block block) { return block & ~1; }
uint8 is_block_free(heap_block block) { return !(block & 1); }

void show_physical_memory() {

  kprint_str("\n------------------KERNEL------------------\n");
  kprintf("multiboot header: %p, size: 0x%x\n", &multiboot_header,
          (uint32)&kernel_text - (uint32)&multiboot_header);

  kprintf("   .text        : %p, size: 0x\n", &kernel_text,
          (uint32)&kernel_rodata - (uint32)&kernel_text);

  kprintf("   .rodata      : %p, size: 0x\n", &kernel_rodata,
          (uint32)&kernel_data - (uint32)&kernel_rodata);

  kprintf("   .data        : %p, size: %x\n", &kernel_data,
          (uint32)&kernel_bss - (uint32)&kernel_data);

  kprintf("   .bss         : %p, size: 0x%x\n", &kernel_bss,
          (uint32)&kernel_end - (uint32)&kernel_bss);

  kprintf("   end of kernel: %p\n", &kernel_end);
  kprint_str("------------------------------------------\n");
}
