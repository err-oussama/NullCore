
int a = 0;
int b;

extern void elf_syscall(unsigned syscall_n);

extern unsigned get_flags();

void _start() {
  b = 0;
  while (b <= 2) {
    b++;
  }
  elf_syscall(1);
}
