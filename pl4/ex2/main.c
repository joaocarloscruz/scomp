#include <stdio.h>      // For printf and standard I/O
#include <stdlib.h>     // For exit, malloc/free (though not strictly needed here for shared memory)
#include <unistd.h>     // For fork, getpid, etc.
#include <sys/mman.h>   // For shared memory functions (shm_open, mmap, munmap, shm_unlink)
#include <sys/stat.h>   // For constants used with shm_open (e.g., S_IRUSR, S_IWUSR)
#include <fcntl.h>      // For file control options (O_CREAT, O_RDWR)
#include <sys/wait.h>   // For wait/waitpid (to wait for the child process)
#include <semaphore.h>  // For POSIX semaphores (sem_open, sem_wait, sem_post, etc.)
#include <errno.h>      // For error handling with errno and perror
//

#define SHM_NAME "/numbers"
#define SEM_NAME "/sem_mutex"
#define RUNTIME 1000000


typedef struct{
	int n1;
	int n2;

} SharedData;


int main(){
	// since the program won't have data consistency without having any mutual exclusion, we'll create a mutex to do that work
	int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	size_t shm_size = sizeof(SharedData);
	if(ftruncate(fd, shm_size) == -1){
		perror("ftruncate failed");
		exit(EXIT_FAILURE);
	}

	SharedData* shared_data = (SharedData*) mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);
	if(shared_data == MAP_FAILED){
        	perror("mmap error");
        	exit(EXIT_FAILURE);
    	}

	shared_data->n1 = 10000;
	shared_data->n2 = 500;

	sem_t* mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
	if (mutex == SEM_FAILED) {
		perror("sem_open failed");
		exit(EXIT_FAILURE);
	}


	pid_t pid;
	pid = fork();
	if(pid < 0){
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	if(pid == 0){
		//child
		for(int i = 0; i < RUNTIME; i++ ){
			sem_wait(mutex);
			shared_data->n1 += 1;
			shared_data->n2 -= 1;
			sem_post(mutex);
		}
		if (sem_close(mutex) == -1) {
			perror("child sem_close failed");
		}
		if (munmap(shared_data, shm_size) == -1) {
			perror("child munmap failed");
		}
		exit(EXIT_SUCCESS);
		
	}
	//child
	for(int i = 0; i < RUNTIME; i++ ){
		sem_wait(mutex);
		shared_data->n1 -= 1;
		shared_data->n2 += 1;
		sem_post(mutex);
	}
	
	waitpid(pid, NULL, 0);
	printf("Value of n1 = %d\nValue of n2 = %d\n",shared_data->n1, shared_data->n2);

	if(sem_close(mutex) == -1){
		perror("mutex closure failed");
		exit(EXIT_FAILURE);
	}

	if (sem_unlink(SEM_NAME) == -1) {
            perror("sem_unlink failed");
            exit(EXIT_FAILURE);
        }
	
	if(munmap(shared_data,shm_size ) == -1){
		perror("unmap failed");
		exit(EXIT_FAILURE);
	}
	if(shm_unlink(SHM_NAME) == -1){
		perror("unlink failed");
		exit(EXIT_FAILURE);
	}

	return 0;
}
