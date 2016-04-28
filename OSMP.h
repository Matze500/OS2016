#ifndef OSMP_H_
#define OSMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int OSMP_Init(int *argc, char ***argv);
extern int OSMP_Size(int *size);
extern int OSMP_Rank(int *rank);
extern int OSMP_Send(const void *buf, int count, int dest);
extern int OSMP_Recv(void *buf, int count,int *source, int *len);
extern int OSMP_Finalize(void);

#endif
