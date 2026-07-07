#include "vmm.h"
#include <control_registers.h>
#include <kernel.h>
#include <kheap.h>
#include <kprint.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <syscall.h>
#include <task.h>
#include <tss.h>

#include <elf.h>

void kmain(multiboot_info *boot_info) {
  init_kernel(boot_info);
  kprintf(
      "======== [Working on: Executable & Linkable Format (ELF)] ========\n\n");
  print_Ehdr(NULL);
}
