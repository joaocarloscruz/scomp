#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

typedef struct {
    char cmd[32];
    int time;
} command_t;

pid_t child_pid;


int main() {
	pid_t pid;	
    command_t commands[] = {
        {"ls", 3}, {"echo Hello!", 3}
        
    };
    int num_commands = sizeof(commands) / sizeof(commands[0]);


    for (int i = 0; i < num_commands; i++) {
	pid = fork();
	if(pid == 0){
        	printf("\nExecuting command: '%s' with timeout: %d seconds\n", commands[i].cmd, commands[i].time);
		sleep(commands[i].time);
	}

 }       
}
