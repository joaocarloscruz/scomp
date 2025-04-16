#ifndef DEFS_H
#define DEFS_H

#define SHM_NAME "/shared_array"
#define ARRAY_SIZE 10

typedef struct{
    volatile int data_available;
    int numbers[ARRAY_SIZE];
} SharedData;

#endif
