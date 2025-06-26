#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define EXCHANGES 30
#define SHM_NAME "/shm_map"
#define SEM_EMPTY_NAME "/sem_empty_buffer"
#define SEM_FULL_NAME "/sem_full_buffer"
#define SEM_MUTEX_NAME "/sem_mutex_buffer"
#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int head;
    int tail;
} SharedData;

int main() {
    pid_t pids[2];
    int shm_fd;
    SharedData *shared_buffer_ptr;
    sem_t *empty_sem;
    sem_t *full_sem;
    sem_t *mutex_sem;

    // Open shared memory for the buffer data
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    shared_buffer_ptr = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_buffer_ptr == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);

    shared_buffer_ptr->head = 0;
    shared_buffer_ptr->tail = 0;

    // Open/Create named semaphores
    empty_sem = sem_open(SEM_EMPTY_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, BUFFER_SIZE);
    if (empty_sem == SEM_FAILED) {
        perror("sem_open empty failed");
        exit(EXIT_FAILURE);
    }

    full_sem = sem_open(SEM_FULL_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    if (full_sem == SEM_FAILED) {
        perror("sem_open full failed");
        exit(EXIT_FAILURE);
    }

    mutex_sem = sem_open(SEM_MUTEX_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (mutex_sem == SEM_FAILED) {
        perror("sem_open mutex failed");
        exit(EXIT_FAILURE);
    }

    pids[0] = fork();
    if (pids[0] < 0) {
        perror("fork producer failed");
        exit(EXIT_FAILURE);
    }
    if (pids[0] == 0) {
        // Producer process
	int value = 0;
	
	for(int i = 0; i < EXCHANGES; i++){
		sem_wait(empty_sem);
		sem_wait(mutex_sem);
		shared_buffer_ptr->buffer[shared_buffer_ptr->head] = value;
		shared_buffer_ptr->head = (shared_buffer_ptr->head + 1) % BUFFER_SIZE;
		sem_post(mutex_sem);
		sem_post(full_sem);
		value++;
	}
        exit(EXIT_SUCCESS);
    }

    pids[1] = fork();
    if (pids[1] < 0) {
        perror("fork consumer failed");
        exit(EXIT_FAILURE);
    }
    if (pids[1] == 0) {
        // Consumer process
	int buff_val;
	for(int i = 0; i < EXCHANGES; i++){
		sem_wait(full_sem);
		sem_wait(mutex_sem);
		printf("Consumed the value %d\n",shared_buffer_ptr->buffer[shared_buffer_ptr->tail]);
		shared_buffer_ptr->tail = (shared_buffer_ptr->tail + 1) % BUFFER_SIZE;
		sem_post(mutex_sem);
		sem_post(empty_sem);
	}

        exit(EXIT_SUCCESS);
    }

    waitpid(pids[0], NULL, 0);
    waitpid(pids[1], NULL, 0);

    // Close named semaphores
    if (sem_close(empty_sem) == -1) {
        perror("sem_close empty failed");
    }
    if (sem_close(full_sem) == -1) {
        perror("sem_close full failed");
    }
    if (sem_close(mutex_sem) == -1) {
        perror("sem_close mutex failed");
    }

    // Unlink named semaphores (this actually removes them from the system)
    if (sem_unlink(SEM_EMPTY_NAME) == -1) {
        perror("sem_unlink empty failed");
    }
    if (sem_unlink(SEM_FULL_NAME) == -1) {
        perror("sem_unlink full failed");
    }
    if (sem_unlink(SEM_MUTEX_NAME) == -1) {
        perror("sem_unlink mutex failed");
    }

    // Unmap and unlink shared memory for the buffer
    if (munmap(shared_buffer_ptr, sizeof(SharedData)) == -1) {
        perror("munmap failed");
    }

    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink failed");
    }

    return 0;
}
