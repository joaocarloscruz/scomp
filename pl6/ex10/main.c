#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
pthread_mutex_t track_AB_lock;
pthread_mutex_t track_BC_lock;
pthread_mutex_t track_BD_lock;


typedef struct {
    int train_id;
    char origin;
    char destination;
    int duration;
} TrainArgs;

void* train_thread(void* args){
	TrainArgs* train_data = (TrainArgs*) args;
	int duration = train_data->duration; 
	if(train_data->destination == 'B'){		
		pthread_mutex_lock(&track_AB_lock);
		printf("Traing [%d] : Starting the trip from City %c to City %c\n", train_data->train_id,train_data->origin, train_data->destination);
		sleep(duration);
		printf("Reached City %c\n", train_data->destination);
		pthread_mutex_unlock(&track_AB_lock);
	
	}
	if(train_data->destination == 'C'){		
		pthread_mutex_lock(&track_BC_lock);
		printf("Traing [%d] : Starting the trip from City %c to City %c\n", train_data->train_id,train_data->origin, train_data->destination);
		sleep(duration);
		printf("Reached City %c\n", train_data->destination);
		pthread_mutex_unlock(&track_BC_lock);
	}
	if(train_data->destination == 'D'){	
		pthread_mutex_lock(&track_BD_lock);
		printf("Traing [%d] : Starting the trip from City %c to City %c\n", train_data->train_id,train_data->origin, train_data->destination);
		sleep(duration);
		printf("Reached City %c\n", train_data->destination);
		pthread_mutex_unlock(&track_BD_lock);
	
	}
	return NULL;
}



int main(){
	if (pthread_mutex_init(&track_AB_lock, NULL) != 0) {
        	perror("Mutex init failed for AB");
        	return 1;
    	}
    	if (pthread_mutex_init(&track_BC_lock, NULL) != 0) {
        	perror("Mutex init failed for BC");
        	return 1;
    	}
    	if (pthread_mutex_init(&track_BD_lock, NULL) != 0) {
        	perror("Mutex init failed for BD");
        	return 1;
    	}
	int count = 0;

	while(1){
		TrainArgs* args = (TrainArgs*)malloc(sizeof(TrainArgs));
		if (args == NULL) {
			perror("Failed to allocate memory for thread args");
			continue; 
		}

		args->train_id = count;

		printf("Enter the train's origin (A, B, C, D):\n");
		scanf(" %c", &args->origin); 

		printf("Enter the train's destination (A, B, C, D):\n");
		scanf(" %c", &args->destination);

		printf("Enter the duration of the trip:\n");
		scanf("%d", &args->duration);

		pthread_t train_tid; 
		pthread_create(&train_tid, NULL, train_thread, (void*)args);	
		count++;

	}
	pthread_mutex_destroy(&track_AB_lock);
	pthread_mutex_destroy(&track_BC_lock);
	pthread_mutex_destroy(&track_BD_lock);


}



