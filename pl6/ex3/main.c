#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct{
	int number;
	char name[50];
	float grade;
} Student;

void* print_student(void* arg){
	Student* s = (Student*) arg;
	printf("Student Number: %d\n", s->number);
	printf("Name: %s\n", s->name);
	printf("Grade: %.2f\n", s->grade);
	printf("------------\n");
	pthread_exit(NULL);
}


int main(){
	pthread_t threads[5];
	Student students[5] = {
        {1, "Joao", 99.5},
        {2, "Luis", 61.0},
        {3, "Pedro", 78.2},
        {4, "Jamie", 54.8},
        {5, "Tiago", 42.4}
	};

	for(int i = 0; i < 5; i++){
		if(pthread_create(&threads[i], NULL, print_student,(void*)&students[i]) != 0 ){
			perror("failed to create thread");
			exit(EXIT_FAILURE);
		}
	}

	for(int i = 0; i < 5; i++){
		pthread_join(threads[i], NULL);
	}

	return 0;
}
