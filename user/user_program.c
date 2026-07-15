
int a = 0;
int b;

extern void elf_syscall(unsigned syscall_n);

extern void write(char *str);

extern unsigned get_flags();

void _start() {
  write("ZZZZZZZZZZZ\n");
  write("ZZZZZZZZZZZ\n");
  write("ZZZZZZZZZZZ\n");
}
