#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>   
#include <sys/stat.h>   
#include <fcntl.h>      

typedef struct{
    int a;
    int b;
} SharedData;

int main(){
    int fd = shm_open(SHM_NAME, O_CREAT | O_RWDR, 0600);



    SharedData *shared = mmap();
    shared->a = 10000;
    shared->b = 500;
    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        exit(-1);
    }
    if( pid == 0) {//child 
    
        for(int i = 0; i < 1000000; i++){
            shared->a++;
            shared->b--;
        }
        exit(1);

    }

    for(int i = 0; i < 1000000; i++){
        shared->a--;
        shared->b++;
    }
    wait(NULL);
    printf("Final Values: a = %d, b = %d\n", shared->a, shared->b);


}
