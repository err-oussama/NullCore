#include "memory.h"
#include "kernel.h"
#include "kprint.h"
#include "multiboot_metadata.h"
#include "pmm.h"
#include "type.h"

static heap_page *heap_memory = NULL; // the first page of the heap

void *get_heap_start() { return heap_memory; }

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
  heap_page *current = heap_memory->next;

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
  size = (size + 3) & ~3;
  if (!block) {
    block = (heap_block *)&page[1];
    *block = size | 1;
    page->free_space -= size + sizeof(heap_block);
    page->last_block = block;
    return block + sizeof(heap_block);
  }

  while (block <= page->last_block) {
    if (is_block_free(*block) && block_size(*block) >= size)
      return &block[1];
    block += block_size(*block);
  }
  if (block + sizeof(heap_block) + size > page + PAGE_SIZE)
    return NULL;

  *block = size | 1;
  page->free_space -= size;
  page->last_block = block;
  return block + sizeof(heap_block);
}
void *kmalloc(uint32 size) {

  if (!heap_memory || !size)
    return NULL;

  heap_page *page = heap_memory;

  /* if (size > PAGE_SIZE)
                   return pages
   */

  while (page->next && page->free_space < size)
    page = page->next;
  if (!page) {
    add_page();
    page = page->next;
    if (!page)
      return NULL;
  }
  return get_block_from_page(page, size);
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
