#include "defs.h"

int main() {
    int fd;
    SharedData* shared_data;
    fd = shm_open(SHM_NAME, O_RDONLY, S_IRUSR | S_IWUSR);
    /*if(ftruncate(fd, DATA_SIZE) < 0){
        perror("truncate error");
        exit(EXIT_FAILURE);
    }*/
    shared_data = mmap(NULL, DATA_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if(shared_data == MAP_FAILED){
        perror("mmap error");
        exit(EXIT_FAILURE);
    }
    while(!shared_data->is_ready){
        sleep(1);
        printf("waiting for data...\n");
    }
    char *name = shared_data->name;
    char *address = shared_data->address;
    int number = shared_data->number;

    printf("name: %s\n", name);
    printf("address: %s\n", address);
    printf("number: %d\n", number);

    return 0;
}
    

    
    