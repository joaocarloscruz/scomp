#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

#define NUMBER_CHILDREN 5
#define NUMBER_POSITIONS 100000
#define SEGMENT_SIZE (NUMBER_POSITIONS / NUMBER_CHILDREN) // Integer division is fine here

void handle_signal(){
	printf("The number has already been found\n");

}

int main() {
    struct sigaction act;
    memset(&act, 0, NULL);
    act.sa_handler = handle_signal;
    act.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &act, NULL);
    int target = 23;

    pid_t pid;
    int* arr = (int*)malloc(NUMBER_POSITIONS * sizeof(int));

    populate_array(arr);

    for (int i = 0; i < NUMBER_CHILDREN; i++) {
        pid = fork();
        if (pid == 0) {
            int start_segment = i * SEGMENT_SIZE;
            int end_segment = (i+1)* SEGMENT_SIZE;

            search_segment(arr, start_segment, end_segment, target, getpid());
            exit(0);
	    if(WEXITSTATUS == 0){
		for(int j = 0; i < NUMBER_CHILDREN; i++){
			kill	kill	kill((
		}	
	    }
        } else if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
    }

    if (pid > 0) {
        for (int i = 0; i < NUMBER_CHILDREN; i++) {
            wait(NULL);
        }
        printf("Parent process: All children finished.\n");
    }

    free(arr);
    return 0;
}

void search_segment(int* arr, int start, int end, int target, int child_pid) {
    for (int i = start; i < end; i++) {
        if (*(arr + i) == target) {
            printf("Child [%d] : Found target number [%d] at position [%d]\n", child_pid, target, i);
	    exit(1);
	}
    }
    printf("Child [%d] : Target number [%d] not found in segment [%d - %d]\n", child_pid, target, start, end);
    exit(0);
}

void populate_array(int* arr) {
    for (int i = 0; i < NUMBER_POSITIONS; i++) {
        *(arr + i) = i + 1;
    }
}
