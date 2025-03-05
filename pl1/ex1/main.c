#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  pid_t p, a;
  p = fork();

  if( p == -1) {
	perror("The was an error with the fork");
	}
  a = fork();
  if( a == -1) {
	perror("There was an error");
	return -1;
  }

  printf("Concurrent Programming\n");
  return 0;
}
