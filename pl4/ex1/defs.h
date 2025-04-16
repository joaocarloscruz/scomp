#ifndef DEFS_H
#define DEFS_H

#define SHM_NAME "/student_info"

typedef struct{
    int data_available;
    int number;
    char name[50];
    char address[100];

} SharedData;

#endif

