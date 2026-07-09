
int a = 0;
int b;
void f1() {

  for (int i = 0; i < 10; i++) {
    if (i == 0)
      b = 10;
    if (i == 9)
      a = 10;
  }
}
