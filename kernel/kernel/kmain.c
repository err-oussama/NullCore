#include <context.h>
#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <string.h>

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();
  cpu_context context;
  cpu_context new;
  memset(&new, 0, sizeof(cpu_context));
  context.eax = 0xAAAAAAAA;
  context.ebx = 0xBBBBBBBB;
  context.ecx = 0xCCCCCCCC;
  context.edx = 0xDDDDDDDD;
  context.edi = 0x99999999;
  context.esi = 0x55555555;

  load_context(&context);
  kprint_hex(save_context(&new));
  kprint_cha('\n');

  kmemory_dump_hex(&new, sizeof(cpu_context));
}
