#ifndef OSMP_H_
#define OSMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include "keys.h"
#include <unistd.h>
#include "structs.h"

#define OSMP_SUCCESS 0
#define OSMP_ERROR -1

#define MUTEX 0
#define LIMIT 1

#define RECEIVED(rank) (rank+2)

extern int OSMP_Init(int *argc, char ***argv);
extern int OSMP_Size(int *size);
extern int OSMP_Rank(int *rank);
extern int OSMP_Send(const void *buf, int count, int dest);
extern int OSMP_Recv(void *buf, int count,int *source, int *len);
extern int OSMP_Finalize(void);

int sem_wait(short semnum);
int sem_signal(short semnum);

#endif
