//2 executables necessary

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../OSMP.h"



void rand_str(char *dest, size_t length);
int testSendRecv(int rank);

int main(int argc, char *argv[]) {

    int rank = -1, source = -1, len = -1, size=-1;

    if (OSMP_Init(&argc, &argv) != OSMP_SUCCESS) {
        printf("OSMP_INIT Error \n");
        exit(EXIT_FAILURE);
    }

    if (OSMP_Size(&size) != OSMP_SUCCESS || size!=2) {
        printf("OSMP_SIZE Error\n");
        exit(EXIT_FAILURE);
    }
    if (OSMP_Rank(&rank) != OSMP_SUCCESS || rank == -1) {
        printf("OSMP_RANK Error \n");
        exit(EXIT_FAILURE);
    }

    char *bufin, *bufout;
    if (rank == 0) {
        sleep(5);
        bufout = malloc(32);
        rand_str(bufout, 32);
        printf("process %d sending random message to process 1: %s\n", rank, bufout);
        if (OSMP_Send(bufout, 32, 1) != OSMP_SUCCESS) {
            printf("osmp_SendRecv Error: OSMP_Send \n");
            exit(EXIT_FAILURE);
        }
        free(bufout);
    } else if(rank==1){
        bufin = malloc(32);
        printf("Process %d trying to receive msg (should block ~5 seconds)\n", rank);
        if (OSMP_Recv(bufin, -10, &source, &len) == OSMP_SUCCESS) {
            printf("Process %d received %d bytes from process %d: %s\n", rank, len, source, bufin);
            printf("Test failed: Recv with NULL buffer successful\n");
            exit(EXIT_FAILURE);
        }
        free(bufin);
    }
   // int ret = testSendRecv(rank);
    if (OSMP_Finalize() != OSMP_SUCCESS) {
        printf("osmp_Finalize Error");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
    //return ret;
}

void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 1) {
        int index = rand() % (int)(sizeof(charset) -1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

int testSendRecv(int rank) {
    int *bufin, *bufout;
    int source, len;
    // OSMP process 0
    if (rank == 0) {
        bufin = malloc(2 * sizeof(int));
        bufin[0] = 4711;
        bufin[1] = 4812;

        if (OSMP_Send(bufin, 2 * sizeof(int), 1) != OSMP_SUCCESS) {
            printf("osmp_SendRecv Error: OSMP_Send \n");
            free(bufin);
            return EXIT_FAILURE;
        }
        free(bufin);
        return EXIT_SUCCESS;
    } else if (rank == 1) {

        // OSMP process 1
        bufout = malloc(2 * sizeof(int));

        if (OSMP_Recv(bufout, 2 * sizeof(int), &source, &len) != OSMP_SUCCESS) {
            printf("osmp_SendRecv Error: OSMP_Recv \n");

            return EXIT_FAILURE;
        }
        printf("OSMP process %d received %d byte from %d [%d:%d] \n", \
    rank, len, source, bufout[0], bufout[1]);

        if (bufout[0] == 4711 && bufout[1] == 4812) {
            printf("Basic Send Recv is working fine\n");
            free(bufout);
            return EXIT_SUCCESS;
        }
    }else if(rank>1){
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
