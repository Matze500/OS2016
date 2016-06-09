#include "OSMP.h"

void *shm;
int semid;

int OSMP_Init(int *argc, char ***argv)
{
  key_t key = createkey(66);
  if(key == (key_t)-1)
    {
      return OSMP_ERROR;
    }
  
  int shmid = shmget(key,0,IPC_PRIVATE);
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
  
  return OSMP_SUCCESS;
}

int OSMP_Size(int *size)
{
  
  if(shm == NULL)
    {
      printf("OSMP_Size_Error\n");
      return OSMP_ERROR;
    }

  if(wait(0) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  struct osmp_info *osinfo = (struct osmp_info *)shm;
  *size = osinfo->processcount;

  if(signal(0) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  return OSMP_SUCCESS;
}

int OSMP_Rank(int *rank)
{

  if(shm == NULL)
    return OSMP_ERROR;
  
  if(wait(0) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  struct osmp_info *osinfo = (struct osmp_info*)shm;
  pid_t pid = getpid();
  
  int i;

  for(i = 0;i < osinfo->processcount;i++)
    {
      if(osinfo->pids[i] == pid)
	{
	  *rank = i;
	  break;
	}
    }

  if(signal(0) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  return OSMP_SUCCESS;
}

int OSMP_Send(const void *buf, int count, int dest)
{ 
  if(count > 128)
    return OSMP_ERROR;

  int rank;
  OSMP_Rank(&rank);
  int size;
  OSMP_Size(&size);
  
  //Pruefen ob maximal Nachrichtenanzahl erreicht
  if(wait(1) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Pruefen ob Mailbox des Empfaengers voll
  if(wait(dest+2+size) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Shared Memory sperren
  if(wait(0) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  struct osmp_info *osinfo = (struct osmp_info*)shm;
  
  size_t osmpsize = osinfo->offset;
  size_t mailboxsize = 2*sizeof(int)+16*sizeof(osmp_message_t);
  
  struct osmp_mailbox *box = (struct osmp_mailbox*)(((char*)shm)+osmpsize+dest*mailboxsize);

  int pos = box->last;
  
  if(box->last == 15)
    {
      box->last = 0;
    }
  else
    {
      box->last++;
    }
  
  box->mailbox[pos].source = rank;
  box->mailbox[pos].dest = dest;
  box->mailbox[pos].length = count;
  
  memcpy(box->mailbox[pos].data,buf,count);
  
  memcpy((((char*)shm)+osmpsize+dest*mailboxsize),box,mailboxsize);

  //Destination neue Nachricht signalisieren
  if(signal(dest+2) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Shared Memory freigeben
  if(signal(0) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  return OSMP_SUCCESS;
}

int OSMP_Recv(void *buf, int count,int *source, int *len)
{
  if(count > 128)
    return OSMP_ERROR;

  printf("Recv: Get rank\n");
  int rank;
  OSMP_Rank(&rank);
  int size;
  OSMP_Size(&size);
  
  //Pruefen ob Nachricht fuer einen da ist
  if(wait(rank+2) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Shared Memory sperren
  if(wait(0) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  struct osmp_info *osinfo = (struct osmp_info*)shm;

  size_t osmpsize = osinfo->offset;
  size_t mailboxsize = 2*sizeof(int)+16*sizeof(osmp_message_t);
  
  struct osmp_mailbox *box = (struct osmp_mailbox*)(((char*)shm)+osmpsize+rank*mailboxsize);
  
  int first = box->first;

  *len = box->mailbox[first].length;
  *source = box->mailbox[first].source;
  
  memcpy(buf,box->mailbox[first].data,count);

  first++;

  memcpy((((char*)shm)+osmpsize+rank*mailboxsize),box,mailboxsize);
  
  //Platz freigeben fuer neue Nachricht im SM
  if(signal(1) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Platz in Mailbox freigeben
  if(signal(rank+2+size) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Shared Memory freigeben
  if(signal(0) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  return OSMP_SUCCESS;
}

int OSMP_Finalize(void)
{ 
  return OSMP_SUCCESS;
}

int wait(short semnum)
{
  static struct sembuf sema;
  sema.sem_num = semnum;
  sema.sem_op = -1;
  return semop(semid,&sema,1);
}

int signal(short semnum)
{
  static struct sembuf sema;
  sema.sem_num = semnum;
  sema.sem_op = 1;
  return semop(semid,&sema,1);
}
