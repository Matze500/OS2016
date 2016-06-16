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

  if(sem_wait(MUTEX) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  struct osmp_info *osinfo = (struct osmp_info *)shm;
  *size = osinfo->processcount;

  if(sem_signal(MUTEX) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  return OSMP_SUCCESS;
}

int OSMP_Rank(int *rank)
{

  if(shm == NULL)
    return OSMP_ERROR;
  
  if(sem_wait(MUTEX) == OSMP_ERROR)
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

  if(sem_signal(MUTEX) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  return OSMP_SUCCESS;
}

int OSMP_Send(const void *buf, int count, int dest)
{ 
  if(count > OSMP_MAX_PAYLOAD_LENGTH)
    return OSMP_ERROR;

  int rank;
  OSMP_Rank(&rank);
  int size;
  OSMP_Size(&size);
  
  //Pruefen ob maximal Nachrichtenanzahl erreicht
  if(sem_wait(LIMIT) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Pruefen ob Mailbox des Empfaengers voll
  if(sem_wait(RECEIVED(dest)+size) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Shared Memory sperren
  if(sem_wait(MUTEX) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  struct osmp_info *osinfo = (struct osmp_info*)shm;
  
  size_t osmpsize = osinfo->offset;
  size_t mailboxsize = sizeof(osmp_mailbox_t);

  //Mailboxen raussuchen
  
  struct osmp_mailbox *freebox = (struct osmp_mailbox*)(((char*)shm)+osmpsize);
  struct osmp_mailbox *destbox = (struct osmp_mailbox*)(((char*)shm)+osmpsize+(dest+1)*mailboxsize);

  int freeplace = freebox->first;
  int destfirst = destbox->first;

  if(destfirst == -1)
    {
      destbox->first = freeplace;
    }
  else
    {
      destbox->last = freeplace;
    }

  printf("Freeplace: %d\n",freeplace);
  
  //Nachricht ersellen
  struct osmp_message *msg = (struct osmp_message*)(((char*)shm)+osmpsize+(size+1)*mailboxsize+freeplace*sizeof(osmp_message_t));

  msg->source = rank;
  msg->length = count;
  memcpy(msg->data,buf,count);
  
  //Naechste freie Stelle suchen
  int free = semctl(semid,1,GETVAL,1);
  
  if(free != 0)
    {
      while(free != -1)
	{
	  freeplace++;
	  struct osmp_message *freemsg = (struct osmp_message*)(((char*)shm)+osmpsize+(size+1)*mailboxsize+freeplace*sizeof(osmp_message_t));
	  free = freemsg->source;
	  if(free == OSMP_MAX_SLOTS)
	    {
	      free = 0;
	    }
	}
      freebox->first = freeplace;
    }
  else
    {
      freebox->first = 0;
    }
  
  //Empfaenger neue Nachricht signalisieren
  if(sem_signal(RECEIVED(dest)) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Shared Memory freigeben
  if(sem_signal(MUTEX) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  return OSMP_SUCCESS;
}

int OSMP_Recv(void *buf, int count,int *source, int *len)
{
  if(count > OSMP_MAX_PAYLOAD_LENGTH)
    return OSMP_ERROR;

  printf("Recv: Get rank\n");
  int rank;
  OSMP_Rank(&rank);
  int size;
  OSMP_Size(&size);
  
  //Pruefen ob Nachricht fuer einen da ist
  if(sem_wait(RECEIVED(rank)) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Shared Memory sperren
  if(sem_wait(MUTEX) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  struct osmp_info *osinfo = (struct osmp_info*)shm;

  size_t osmpsize = osinfo->offset;
  size_t mailboxsize = sizeof(osmp_mailbox_t);
  size_t messagesize = sizeof(osmp_message_t);
  
  struct osmp_mailbox *box = (struct osmp_mailbox*)(((char*)shm)+osmpsize+(rank+1)*mailboxsize);

  printf("First: %d\n",box->first);
  
  int first = box->first;
  int last = box->last;

  struct osmp_message *msg = (struct osmp_message*)(((char*)shm)+osmpsize+(size+1)*mailboxsize+first*messagesize);
  
  *len = msg->length;
  *source = msg->source;
  memcpy(buf,msg->data,count);
  
  box->first = msg->next;
  if(msg->next == last)
    {
      box->last = -1;
    }

  msg->next = -1;
  msg->source = -1;
  
  //Freien Platz in die passende Mailbox eintragen
  struct osmp_mailbox *free = (struct osmp_message*)(((char*)shm)+osmpsize);
  free->first = first;
  
  //Platz freigeben fuer neue Nachricht im SM
  if(sem_signal(LIMIT) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Platz in Mailbox freigeben
  if(sem_signal(RECEIVED(rank)+size) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  
  //Shared Memory freigeben
  if(sem_signal(MUTEX) == OSMP_ERROR)
    {
      return OSMP_ERROR;
    }
  return OSMP_SUCCESS;
}

int OSMP_Finalize(void)
{ 
  return OSMP_SUCCESS;
}

int sem_wait(short semnum)
{
  static struct sembuf sema;
  sema.sem_num = semnum;
  sema.sem_op = -1;
  return semop(semid,&sema,1);
}

int sem_signal(short semnum)
{
  static struct sembuf sema;
  sema.sem_num = semnum;
  sema.sem_op = 1;
  return semop(semid,&sema,1);
}
