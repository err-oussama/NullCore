
int a = 0;
int b;

extern void elf_syscall();

void _start() {
  b = 0;
  elf_syscall();
  while (b <= 2) {
    if (!a++) {
      elf_syscall();
      b++;
    }
    if (a == 10000)
      a = 0;
  }
  elf_syscall();
}
