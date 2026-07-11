
int a = 0;
int b;

extern void elf_syscall();

void _start() {
  elf_syscall();
  while (1) {
  }
}
