#include "defs.h"

int main() {    
    SharedData* shared_data;
    int fd;
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if(ftruncate(fd, DATA_SIZE) < 0){
        perror("truncate error");
        exit(EXIT_FAILURE);
    }
    shared_data = mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shared_data == MAP_FAILED){
        perror("mmap error");
        exit(EXIT_FAILURE);
    }
    shared_data->is_ready = 0;

    //read data
    char *name = shared_data->name;
    char *address = shared_data->address;
    int num;
    size_t len = 50; 
    printf("Insert the student name:\n");
    if(getline(&name, &len, stdin) < 0){
        perror("read error");
        exit(EXIT_FAILURE);
    }
    printf("name: %s\n", name);

    printf("Insert the student number\n");
    if(scanf("%d", &num) < 0){
        perror("read error");
        exit(EXIT_FAILURE);
    }
    printf("number: %d\n", num);
    shared_data->number = num;

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    len = 100;
    printf("Insert the student address \n");
    if(getline(&address, &len , stdin) < 0){
        perror("read error");
        exit(EXIT_FAILURE);
    }
    printf("address: %s\n", address);


    shared_data->is_ready = 1;
    printf("Finished reading\n");

    //detach

    return 0;
}