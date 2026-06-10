#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <task.h>

#define T 0x10000000
uint32 id = 0;

void task1() {
  uint64 i = 0;
  id++;
  kprint_str("TASK ");
  kprint_dec(id);
  kprint_str(" at: ");
  kprint_hex64(pit_get_tick());
  kprint_str("\n");
}

void kernel_task() {
  uint64 i = 0;
  kprint_str("TASK 0: ");
  kprint_hex64(pit_get_tick());
  kprint_str("\n");
  while (1) {
    i++;
    if (i == T) {
      kprint_str("TASK 0: ");
      kprint_hex64(pit_get_tick());
      kprint_str("\n");
      i = 0;
    }
  }
}
void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();

  task_init();
  create_task(task1);
  create_task(task1);
  create_task(task1);
  create_task(task1);
  kernel_task();
}
