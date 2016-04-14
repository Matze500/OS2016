#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int OSMP_Init(int *argc, char ***argv)
{
  printf("OSMP_Init\n");
  return 0;
}

int OSMP_Size(int *size)
{
  printf("OSMP_Size\n");
  return 0;
}

int OSMP_Rank(int *rank)
{
  printf("OSMP_Rank\n");
  return 0;
}

int OSMP_Send(const void *buf, int count, int dest)
{
  printf("OSMP_Send\n");
  return 0;
}

int OSMP_Recv(void *buf, int count,int *source, int *len)
{
  printf("OSMP_Recv\n");
  return 0;
}

int OSPM_Finalize(void)
{
  printf("OSMP_Finalize\n");
  return 0;
}
