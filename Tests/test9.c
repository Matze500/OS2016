//34 executable necessary

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../OSMP.h"


void rand_str(char *dest, size_t length);

int main(int argc, char *argv[]) {

    int source, len;
    int rank = -1, size=-1;

    if (OSMP_Init(&argc, &argv) != OSMP_SUCCESS) {
        printf("OSMP_INIT Error \n");
        exit(EXIT_FAILURE);
    }

    if (OSMP_Size(&size) != OSMP_SUCCESS || size!=34) {
        printf("OSMP_SIZE Error\n");
        exit(EXIT_FAILURE);
    }

    if (OSMP_Rank(&rank) != OSMP_SUCCESS) {
        printf("OSMP_RANK Error \n");
        exit(EXIT_FAILURE);
    }

    char *bufin, *bufout;
    if (rank < 16) {
        for (int i = 0; i < 16; i++) {
            bufout = malloc(32);
            rand_str(bufout, 32);
            printf("process %d sending random message to process %d: %s\n", rank, rank + 17, bufout);
            if (OSMP_Send(bufout, 32, rank + 17) != OSMP_SUCCESS) {

                printf("osmp_SendRecv Error: OSMP_Send \n");
                exit(EXIT_FAILURE);

            }
            free(bufout);
        }
    } else if (rank == 16) {
        sleep(5);
        for (int i = 0; i < 16; i++) {
            bufout = malloc(32);
            rand_str(bufout, 32);
            printf("process %d sending random message to process %d: %s (should block, no free msg slots)\n", rank,
                   rank + 17, bufout);
            if (OSMP_Send(bufout, 32, rank + 17) != OSMP_SUCCESS) {

                printf("osmp_SendRecv Error: OSMP_Send \n");
                exit(EXIT_FAILURE);

            }
            puts("Process 16, msg was sent");

            free(bufout);
        }
    } else if (rank<34){
        sleep(10);
        for (int j = 0; j < 16; j++) {
            bufin = malloc(32);
            printf("Process %d trying to receive msg\n", rank);
            if (OSMP_Recv(bufin, 32, &source, &len) != OSMP_SUCCESS) {
                printf("osmp_SendRecv Error: OSMP_Recv \n");

                exit(EXIT_FAILURE);
            }
            printf("Process %d received %d bytes from process %d: %s\n", rank, len, source, bufin);
            free(bufin);
        }


    }
    if (OSMP_Finalize() != OSMP_SUCCESS) {
        printf("osmp_Finalize Error");
        exit(EXIT_FAILURE);
    }
    return 0;
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
