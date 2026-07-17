int a = 0;
int b;

extern void syscall(unsigned syscall_n, void *buff);

char *str = "0123456789ABCDEF\n";
void _start() {
  char *Str = "01234567\n";
  syscall(1, "1 one\n");
  syscall(1, "2 one\n");
  syscall(1, "3 one\n");
  syscall(1, "4 one\n");
  syscall(1, "5 one\n");
  syscall(1, str);
  syscall(1, Str);
  syscall(1, str);
  syscall(1, Str);
}
