#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

pid_t create_child(int num_filho);

int main(){
	int num_filho=1;
	int status1, status2;
	pid_t pid1, pid2;

	pid1 = create_child(num_filho);
	num_filho++;
	pid2 = create_child(num_filho);


	printf("Eu sou o pai e tenho o pid [%d]\n", getpid());
	wait(&status1);
	printf("Pai : o filho retornou o valor: %d\n", WEXITSTATUS(status1));
	wait(&status2);
	printf("Pai: o filho retornou o valor: %d\n", WEXITSTATUS(status2));

	return 0;
}


pid_t create_child(int num_filho){
	pid_t pid = fork();
	if(pid < 0){
		perror("Erro ao criar o processo\n");
		exit(-1);
	}

	if(pid > 0) return pid;

	printf("Eu sou o %d filho com o pid = %d \n", num_filho,getpid());
	sleep(5*num_filho);
	exit(num_filho);
}
