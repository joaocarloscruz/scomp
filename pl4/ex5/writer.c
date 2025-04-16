#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>   
#include <sys/stat.h>   
#include <fcntl.h>    

#define V_SIZE 1000
#define MAX_SIZE 50
#define BLOCK_SIZE (V_SIZE / MAX_SIZE)

typedef struct{
    int v[V_SIZE];
    int max[MAX_SIZE];


} SharedData;

int main(){

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);


    if(ftruncate(fd,sizeof(SharedData)) == -1)


     SharedData *shared_data


     // initialize random vectore
     //
     srand(time(NULL));
for

    //create 10 child processes
    //
    for(int i
    if(pid == 0){
        int local_max = 0;
        for(int j = i * BLOCK_SIZE; j < (i+1 ) * BLOCK_SIZE; j++){
            if(shred_data->v[] > local_max){
                local_max = shared_data->v[i];
            }
        shared_data->max[i] = local_max;
        exit(1);
        }

    }


    return 0;
}
