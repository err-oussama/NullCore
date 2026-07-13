#include <control_registers.h>
#include <elf.h>
#include <kernel.h>
#include <kheap.h>
#include <kprint.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <registers.h>
#include <syscall.h>
#include <task.h>
#include <tss.h>

#include <syscall.h>

void task_mock() { kprintf("t"); }

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprintf(
      "======== [Working on: Executable & Linkable Format (ELF)] ========\n\n");

  /* load_elf((void *)initrd_start); */
  kprintf("first\n");
  for (uint32 i = 0; i < 0x10000000; i++)
    ;
  kprintf("second\n");
  load_elf((void *)initrd_start);
}
