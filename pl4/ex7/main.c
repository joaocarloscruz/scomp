#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#define NR_COURSES 10
#define STR_SIZE 50
typedef struct{
    int number;
    char name[STR_SIZE];
    int courses[NR_COURSES];
    int is_ready;
}student_t;

int main(){
    int fd;
    pid_t pid[2];
    student_t* shared_data;
    fd = shm_open("/student", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(ftruncate(fd, sizeof(student_t)) < 0){
        perror("truncate error");
        exit(EXIT_FAILURE);
    }
    shared_data = mmap(NULL, sizeof(student_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shared_data == MAP_FAILED){
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 2; i++){
        pid[i] = fork();

        if(pid[i] < 0){
            perror("fork error");
            exit(EXIT_FAILURE);
        }

        if(pid[i] == 0){
            while(shared_data->is_ready != 1);
            if(i == 0){
                int max = shared_data->courses[0];
                int min = shared_data->courses[0];

                for(int j = 0; j < NR_COURSES; j++){
                    if(shared_data->courses[j] > max){
                        max = shared_data->courses[j];
                    }
                    if(shared_data->courses[j] < min){
                        min = shared_data->courses[j];
                    }
                }
                printf("max: %d\n", max);
                printf("min: %d\n", min);

            }
            if(i == 1){
                int sum = 0;
                for(int j = 0; j < NR_COURSES; j++){
                    sum += shared_data->courses[j];
                }
                double average = (double)sum / NR_COURSES;
                printf("average: %f\n", average);


            }
            exit(EXIT_FAILURE);

        }
    }

    // parent 
    // user input values
    char* name;
    size_t len = STR_SIZE;
    int number;
    int courses[NR_COURSES];

    printf("Insert the student name:\n");
    if(getline(&name, &len , stdin) < 0){
        perror("read error");
        exit(EXIT_FAILURE);
    }
    printf("name: %s\n", name);
    printf("Insert the student number\n");
    if(scanf("%d", &number) < 0){
        perror("read error");
        exit(EXIT_FAILURE);
    }
    printf("number: %d\n", number);
    // empty buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Insert the student courses\n");
    for(int i = 0; i < NR_COURSES; i++){
        if(scanf("%d", &courses[i]) < 0){
            perror("read error");
            exit(EXIT_FAILURE);
        }
        printf("course: %d\n", courses[i]);
    }
    shared_data->number = number;
    strcpy(shared_data->name, name);
    for(int i = 0; i < NR_COURSES; i++){
        shared_data->courses[i] = courses[i];
    }
    shared_data->is_ready = 1;
    
    //detach
    if(munmap(shared_data, sizeof(student_t)) < 0){
        perror("munmap error");
        exit(EXIT_FAILURE);
    }
    if(close(fd) < 0){
        perror("close error");
        exit(EXIT_FAILURE);
    }
    if(shm_unlink("/student") < 0){
        perror("shm_unlink error");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 2; i++){
        wait(NULL);
    }

    printf("Finished the process\n");
    return 0;
}