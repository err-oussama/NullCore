#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <task.h>

void task1() {
  kprint_str("\nswitched to the task1\n");
  while (1)
    ;
}

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();

  task_init();
  create_task(task1);
  kprintf("\nafter creating task");
}
