#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

void thread_func(void* arg) {
    char* str = (char*)arg;
    printf("%s\n", str);
    pthread_exit(NULL);
}

int main() {
    char first_name[] = "João";
    char last_name[] = "Cruz";

    pthread_t thread[2];

    pthread_create(&thread[0], NULL, thread_func, (void*)first_name);

    pthread_create(&thread[1], NULL, thread_func, (void*)last_name);

    for (int i = 0; i < 2; i++) {
        pthread_join(thread[i], NULL);
    }

    return 0;
}
