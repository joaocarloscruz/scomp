#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
void handle_signal(){
	printf("The divisor is 0. The program will shutdown");
	exit(-1);
}

int main(){
	int a, b;
	struct sigaction act; 

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler= handle_signal;
	act.sa_flags = SA_RESTART;
	sigaction(SIGFPE, &act, NULL);

	printf("Insert the dividend:");
	scanf("%d", &a);

	printf("Insert the divisor:");
	scanf("%d", &b);


	float result = (float) a / b;
	printf("The result of the division is %f\n", result);

	return 0;
}


