#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <task.h>

void task1() {
  kprint_str("\ntask 1 is running\n");
  while (1)
    ;
}
void task2() {
  kprint_str("\ntask 2 is running\n");
  while (1)
    ;
}

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();

  task_init();
  create_task(task2);
  create_task(task1);
  kprintf("\nafter creating task");
}
