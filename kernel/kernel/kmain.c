#include <kernel.h>
#include <kprint.h>
#include <memory.h>
#include <multiboot_metadata.h>
#include <pit.h>
#include <task.h>

void task1() {
  kprint_str("\nhellow\n");
  kprint_str("hellow\n");

  while (1)
    ;
}

void kmain(multiboot_info *boot_info) {
  kclear_screen();
  init_pmp(boot_info);
  setup_hardware();
  init_heap();

  uint32 cs, eflags;
  asm("mov %%cs, %0" : "=r"(cs));
  asm("pushf; pop %0" : "=r"(eflags));
  kprintf("cs: '%p'    eflags: '%p'\n", cs, eflags);

  task_init();
  create_task(task1);
  kprintf("\nafter creating task");
}
