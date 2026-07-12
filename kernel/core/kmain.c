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

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprintf(
      "======== [Working on: Executable & Linkable Format (ELF)] ========\n\n");

  load_elf((void *)initrd_start);
}
