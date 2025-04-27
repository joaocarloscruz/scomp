#ifndef DEFS_H
#define DEFS_H

#define SHM_NAME "/student_info"
#define DATA_SIZE 200

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef struct{
    char address[100];
    char name[50];
    int number;
    int is_ready;
} SharedData;

#endif

