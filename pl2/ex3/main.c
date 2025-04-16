#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void handle_signal(){
	perror("I got signal SIGSEGV");
	exit(21);
}
int main(){
	int a;
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = handle_signal;
	act.sa_flags = SA_RESTART;

	sigaction(SIGSEGV, &act, NULL);
	
	a = *(int *) 0;
}
