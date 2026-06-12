#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <syscall.h>
#include <task.h>

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();
  task_init();
  syscall_enter(0x0, 0xB000000B, 0xC000000C, 0xD000000D, 0x50000001, 0xD0000001,
                0xD0000009);
}
