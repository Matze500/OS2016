#include "OSMP.h"

void *shm;
int semid;

static struct sembuf semaphore;

int OSMP_Init(int *argc, char ***argv)
{
  key_t key = createkey(666);
  if(key == (key_t)-1)
    {
      return OSMP_ERROR;
    }
  int shmid = shmget(key,1024,IPC_PRIVATE);
  if(shmid == -1)
    {
      return OSMP_ERROR;
    }
  shm = shmat(shmid,NULL,0);
   if(shm == (void*)-1)
    {
      return OSMP_ERROR;
    }

  semid = semget(key,0,IPC_PRIVATE);
  if(semid < 0)
    {
      return OSMP_ERROR;
    }
  
  printf("OSMP_Init_Call\n");
  return OSMP_SUCCESS;
}

int OSMP_Size(int *size)
{
  if(shm == NULL)
    {
      printf("OSMP_Size_Error\n");
      return OSMP_ERROR;
    }
  
  struct osmp_info *osinfo = (struct osmp_info *)shm;
  *size = osinfo->processcount;
  printf("OSMP_Size_Call\n");
  return OSMP_SUCCESS;
}

int OSMP_Rank(int *rank)
{

  if(shm == NULL)
    return OSMP_ERROR;
  
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
  return OSMP_SUCCESS;
}

int OSMP_Send(const void *buf, int count, int dest)
{
  int ret = 0;

  if(count > 128)
    return OSMP_ERROR;
  
  //Pruefen ob maximal Nachrichtenanzahl erreicht
  int messagespace = semctl(semid,1,GETVAL,0);
  if(messagespace < 0)
    {
      return OSMP_ERROR;
    }
  else
    {
      while(messagespace == 0)
	{
	  messagespace = semctl(semid,1,GETVAL,0);
	}
      ret = semctl(semid,1,SETVAL,messagespace-1);
      if(ret < 0)
	{
	  return OSMP_ERROR;
	}
    }

  //Pruefen ob Messagebox des Empfaengers voll
  int boxspace = semctl(semid,dest+2,GETVAL,0);
  if(boxspace < 0)
    {
      return OSMP_ERROR;
    }
  else
    {
      while(boxspace == 0)
	{
	  boxspace = semctl(semid,dest+2,GETVAL,0);
	}
      ret = semctl(semid,dest+2,SETVAL,boxspace-1);
      if(ret < 0)
	{
	  return OSMP_ERROR;
	}
    }
  
  //Shared Memory sperren
  if(semoperation(LOCK) == -1)
    {
      return OSMP_ERROR;
    }
  
  int rank;
  OSMP_Rank(&rank);
  
  struct osmp_info *osinfo = (struct osmp_info*)shm;
  
  size_t osmpsize = osinfo->offset;
  size_t mailboxsize = sizeof(osmp_mailbox_t);
  
  struct osmp_mailbox *box = (struct osmp_mailbox*)shm+osmpsize+dest*mailboxsize;

  int first = box->first;
  int last = box->last;

  if(last == 15)
    {
      last = 0;
    }
  else
    {
      last++;
    }

  box->mailbox[last]->source = rank;
  box->mailbox[last]->dest = dest;
  box->mailbox[last]->length = count;
  memcpy(box->mailbox[last]->data,buf,count);

  memcpy(shm+osmpsize+dest*mailboxsize,box,mailboxsize);
  
  //Shared Memory freigeben
  if(semoperation(UNLOCK) == -1)
    {
      return OSMP_ERROR;
    }
  printf("OSMP_Send_Call\n");
  return OSMP_SUCCESS;
}

int OSMP_Recv(void *buf, int count,int *source, int *len)
{
  if(count > 128)
    return OSMP_ERROR;
  
  int ret = 0;
  //Shared Memory sperren
  if(semoperation(LOCK) == -1)
    {
      return OSMP_ERROR;
    }
  
  struct osmp_info *osinfo = (struct osmp_info*)shm;

  size_t osmpsize = osinfo->offset;
  size_t mailboxsize = sizeof(osmp_mailbox_t);

  int rank;
  OSMP_Rank(&rank);
  
  struct osmp_mailbox *box = (struct osmp_mailbox*)shm+osmpsize+rank*mailboxsize;

  int first = box->first;
  int last = box->last;

  *len = box->mailbox[first]->length;
  *source = box->mailbox[first]->source;
  
  memcpy(buf,box->mailbox[first],count);

  first++;

  memcpy(shm+osmpsize+rank*mailboxsize,box,mailboxsize);
  
  //Platz freigeben fuer neue Nachricht
  int messagespace = semctl(semid,1,GETVAL,0);
  if(messagespace < 0)
    {
      return OSMP_ERROR;
    }
  else
    {
      ret = semctl(semid,1,SETVAL,messagespace+1);
      if(ret < 0)
	{
	  return OSMP_ERROR;
	}
    }
  //Platz in Messagebox freigeben

  int boxspace = semctl(semid,rank+2,GETVAL,0);
  if(boxspace < 0)
    {
      return OSMP_ERROR;
    }
  else
    {
      ret = semctl(semid,rank+2,SETVAL,boxspace+1);
      if(ret < 0)
	{
	  return OSMP_ERROR;
	}
    }
  
  //Shared Memory freigeben
  if(semoperation(UNLOCK) == -1)
    {
      return OSMP_ERROR;
    }
  
  printf("OSMP_Recv_Call\n");
  return OSMP_SUCCESS;
}

int OSMP_Finalize(void)
{ 
  printf("OSMP_Finalize_Call\n");
  return OSMP_SUCCESS;
}

int semoperation(short op)
{
  semaphore.sem_num = 0;
  semaphore.sem_op = op;
  semaphore.sem_flg = SEM_UNDO;
  return semop(semid,&semaphore,1);
}
