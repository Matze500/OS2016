//270 executables necessary

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../OSMP.h"


void rand_str(char *dest, size_t length);

int main(int argc, char *argv[]) {

    int rank = -1, size = -1;


    if (OSMP_Init(&argc, &argv) != OSMP_SUCCESS) {
        printf("OSMP_INIT Error \n");
        exit(EXIT_FAILURE);
    }

    if (OSMP_Rank(&rank) != OSMP_SUCCESS) {
        printf("OSMP_RANK Error \n");
        exit(EXIT_FAILURE);
    }

    if (OSMP_Size(&size) != OSMP_SUCCESS || size !=270) {
        printf("OSMP_SIZE Error\n");
        exit(EXIT_FAILURE);
    }

    char *bufin, *bufout;
    if (rank > 1) {
        bufout = malloc(32);
        rand_str(bufout, 32);
        printf("process %d sending random message to process 0: %s\n", rank, bufout);
        if (OSMP_Send(bufout, 32, 0) != OSMP_SUCCESS) {

            printf("OSMP_SEND Error \n");
            exit(EXIT_FAILURE);

        }
        free(bufout);

    } else {
        if (rank == 0) {
            sleep(5);
            bufout = malloc(32);
            puts("Process 0 trying to send msg to process 1 (should not block)");
            if (OSMP_Send(bufout, 32, 1) != OSMP_SUCCESS) {
                printf("OSMP_SEND Error\n");
                exit(EXIT_FAILURE);
            }
            puts("Msg from process 0 was sent");
            free(bufout);

            bufin = malloc(32);
            int source, len;
            puts("Process 0 reading all msges");
            for (int i = 0; i < size - 2; i++) {
                rand_str(bufin, 32);
                if (OSMP_Recv(bufin, 32, &source, &len) != OSMP_SUCCESS) {
                    printf("OSMP_RECV Error \n");

                    exit(EXIT_FAILURE);
                }
            }
        }
        if (rank == 1) {
            puts("Process 1 reading one msg from process 0");
            bufin = malloc(32);
            int source, len;
            rand_str(bufin, 32);
            if (OSMP_Recv(bufin, 32, &source, &len) != OSMP_SUCCESS) {
                printf("OSMP_RECV Error \n");

                exit(EXIT_FAILURE);

            }
        }
    }
    if (OSMP_Finalize() != OSMP_SUCCESS) {
        printf("OSMP_FINALIZE Error");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
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
