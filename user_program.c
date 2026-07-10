
int a = 0;
int b;

extern void elf_syscall();

void f1() {

  elf_syscall();
  for (int i = 0; i < 10; ++i) {
    if (i == 0)
      b = 10;
    if (i == 9)
      a = 10;
    --i;
  }
}
