#include "OSMP.h"

int OSMP_Init(int *argc, char ***argv)
{
  printf("OSMP_Init_Call\n");
  return 0;
}

int OSMP_Size(int *size)
{
  printf("OSMP_Size_Call\n");
  return 0;
}

int OSMP_Rank(int *rank)
{
  printf("OSMP_Rank_Call\n");
  return 0;
}

int OSMP_Send(const void *buf, int count, int dest)
{
  printf("OSMP_Send_Call\n");
  return 0;
}

int OSMP_Recv(void *buf, int count,int *source, int *len)
{
  printf("OSMP_Recv_Call\n");
  return 0;
}

int OSMP_Finalize(void)
{
  printf("OSMP_Finalize_Call\n");
  return 0;
}
