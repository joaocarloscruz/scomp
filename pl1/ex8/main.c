#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
  int a, b, c, d;

  a = 0;
  b = fork();
  c = getpid();
  d = getppid();
  a = a + 5;
  printf("\na =%d , b =%d , c =%d , d =%d\n", a, b, c, d);
  return 0; // Added return statement for good practice

  // a will return 5 in both processes.
  // for the rest of the variables, the value will be always different.
}