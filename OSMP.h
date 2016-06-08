#ifndef OSMP_H_
#define OSMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "keys.h"
#include <unistd.h>
#include "structs.h"

#define OSMP_SUCCESS 0
#define OSMP_ERROR -1

#define UNLOCK 1
#define LOCK -1 

#define OSMP_MAX_MESSAGES_PROC 16
#define OSMP_MAX_SLOTS 256
#define OSMP_MAX_PAYLOAD_LENGTH 128

extern int OSMP_Init(int *argc, char ***argv);
extern int OSMP_Size(int *size);
extern int OSMP_Rank(int *rank);
extern int OSMP_Send(const void *buf, int count, int dest);
extern int OSMP_Recv(void *buf, int count,int *source, int *len);
extern int OSMP_Finalize(void);

int semoperation(short op);

#endif
