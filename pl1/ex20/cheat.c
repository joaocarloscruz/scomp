#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){
    srand(time(NULL) ^ getpid());
    int number = (rand() % 5) + 1;
    //printf("generated number %d", number);
    return number;
}