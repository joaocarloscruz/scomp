#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define NUMBER_CHILDREN 10
#define NUMBER_ENTRIES 10000
#define POSITIONS_CHILD (NUMBER_ENTRIES / NUMBER_CHILDREN)

pid_t child_pid[NUMBER_CHILDREN];
volatile sig_atomic_t region_found = 0;

typedef struct{
	char name[50];
	char region[50];
	int phone;
} record_t;

// Dummy function to simulate reading the database
void populate_database(record_t array) {
    // In a real application, you would read this from a file or database
    for (int i = 0; i < NUMBER_ENTRIES; i++) {
        sprintf(array[i].name, "Workshop %d", i);
        if (i % 50 == 0) {
            strcpy(array[i].region, "Lisbon");
            array[i].phone = 123456789 + i;
        } else if (i % 75 == 0) {
            strcpy(array[i].region, "Porto");
            array[i].phone = 987654321 + i;
        } else {
            strcpy(array[i].region, "Other Region");
            array[i].phone = 555123456 + i;
        }
    }
}

void handle_signal(int sig){
	if (sig == SIGUSR1) {
		region_found = 1;
	}
}

int main(){
	pid_t pid;
	struct sigaction act;
	act.sa_handler = handle_signal;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIGUSR1, &act, NULL);

	record_t array[NUMBER_ENTRIES];
	populate_database(&array[0]); // Simulate reading the database
	int count = 0;
	char region_input[50];

	printf("Enter the name of your region\n");
	scanf("%s", region_input);

	for (count = 0; count < NUMBER_CHILDREN; count++) {
		pid = fork();
		child_pid[count] = pid;
		if (pid < 0) {
			perror("Error with the fork");
			for (int i = 0; i < count; i++) {
				kill(child_pid[i], SIGKILL);
			}
			return 1;
		} else if (pid == 0) {
			int start = count * POSITIONS_CHILD;
			int end = (count == NUMBER_CHILDREN - 1) ? NUMBER_ENTRIES : (count + 1) * POSITIONS_CHILD;

			for (int i = start; i < end; i++) {
				if (strcmp(array[i].region, region_input) == 0) {
					printf("Workshop found by process %d: Name: %s, Phone: %d\n", getpid(), array[i].name, array[i].phone);
					kill(getppid(), SIGUSR1);
					exit(0);
				}
			}
			exit(1);
		}
	}

	int status;
	pid_t child;
	for (int i = 0; i < NUMBER_CHILDREN; i++) {
		child = wait(&status);
		if (region_found) {
			for (int j = 0; j < NUMBER_CHILDREN; j++) {
				if (child_pid[j] != child && child_pid[j] > 0) {
					kill(child_pid[j], SIGKILL);
				}
			}
			break;
		}
	}

	printf("Search complete.\n");

	return 0;
}
