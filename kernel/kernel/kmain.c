#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <task.h>

void task1() {
  uint64 i = 0;
  kprint_str("TASK 1 RUNNING\n");
  while (1) {
    i++;
    if (i == 0x10000000) {

      kprint_str("TASK 1 RUNNING\n");
      i = 0;
    }
  }
}

void task2() {
  uint64 i = 0;
  kprint_str("TASK 2 RUNNING\n");
  while (1) {
    i++;
    if (i == 0x10000000) {

      kprint_str("TASK 2 RUNNING\n");
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
  create_task(task2);
}
