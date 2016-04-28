#include "OSMP.h"

void *shm;

int OSMP_Init(int *argc, char ***argv)
{
  key_t key = createkey(142);
  if(key == (key_t)-1)
    {
      return -1;
    }
  int shmid = shmget(key,1024,0666);
  if(shmid == -1)
    {
      return -1;
    }
  shm = shmat(shmid,NULL,0);
   if(shm == (void*)-1)
    {
      return -1;
    }
  printf("OSMP_Init_Call\n");
  return 0;
}

int OSMP_Size(int *size)
{
  struct osmp_info *osinfo = (struct osmp_info *)shm;
  *size = osinfo->processcount;
  printf("OSMP_Size_Call\n");
  return 0;
}

int OSMP_Rank(int *rank)
{
  struct osmp_info *osinfo = (struct osmp_info*)shm;
  pid_t pid = getpid();

  int i;

  for(i = 0;i < osinfo->processcount;i++)
    {
      if(osinfo->pids[i] == pid)
	{
	  *rank = i;
	  return -1;
	}
    }
  
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
