//2 executables necesary

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../OSMP.h"


int main(int argc, char *argv[]) {
    int size=-1, rank=-1;
    int *n = NULL;
    if (OSMP_Init(&argc, &argv) != OSMP_SUCCESS) {
        printf("OSMP_INIT Error\n");
        exit(EXIT_FAILURE);
    }

    if (OSMP_Size(&size) != OSMP_SUCCESS || size!=2) {
        printf("OSMP_SIZE Error 1\n");
        exit(EXIT_FAILURE);
    }

    if (OSMP_Rank(&rank) != OSMP_SUCCESS || rank==-1) {
        printf("OSMP_RANK Error 1\n");
        exit(EXIT_FAILURE);
    }
    
    if (OSMP_Size(n) == OSMP_SUCCESS) {
        printf("OSMP_SIZE Error 2\n");
        exit(EXIT_FAILURE);
    }

    if (OSMP_Rank(n) == OSMP_SUCCESS) {
        printf("OSMP_RANK Error 2\n");
        exit(EXIT_FAILURE);
    }

    if(OSMP_Finalize()!=OSMP_SUCCESS){
        printf("OSMP_FINALIZE Error");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
