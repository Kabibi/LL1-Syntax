//
// Created by aaron on 17-4-25.
//

#ifndef COMPILER_SET_H
#define COMPILER_SET_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXSIZE 100
#define PROD_NUM 5
#define MAX_REC 20

typedef struct Prod {
    char head[MAXSIZE];
    char body[MAXSIZE][MAXSIZE];
    int n;
} Prod;

typedef struct Set {
    char data[MAXSIZE];
    int n;
} Set;

#endif //COMPILER_SET_H
