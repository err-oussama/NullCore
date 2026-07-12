
int a = 0;
int b;

extern void elf_syscall();

void _start() {
  while (b <= 5) {
    if (!a++) {
      elf_syscall();
      b++;
    }
    if (a == 10000000)
      a = 0;
  }
}
