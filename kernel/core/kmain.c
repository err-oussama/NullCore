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
  kprint_wrn("======== [Working on: User Process] ========\n");
  void *elf = initrd_start;

  load_elf(elf);
  load_elf(elf);
  load_elf(elf);
  load_elf(elf);
}
