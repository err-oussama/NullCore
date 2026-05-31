#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <task.h>

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();
  cpu_context context;
  cpu_context new;
  new.eax = 0xAA;
  new.ebx = 0xBB;
  new.ecx = 0xCC;
  new.edx = 0xDD;
  new.edi = 0x99;
  new.esi = 0x55;
  context.eax = 0xff;
  load_context(&context);
  save_context(&new);

  kprintf("eax: %x\nebx: %x\necx: %x\nedx: %x\nedi: %x\nesi: %x\n", new.eax,
          new.ebx, new.ecx, new.edx, new.edi, new.esi);
}
