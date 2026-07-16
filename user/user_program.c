
int a = 0;
int b;

extern void elf_syscall(unsigned syscall_n);

extern void write(char *str);

extern unsigned get_flags();

char *str = "0123456789ABCDEF\n";
void _start() {
  elf_syscall(0);
  write(str);
  write("hellow");
  write("heow");
}
