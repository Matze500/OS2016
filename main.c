/* main.c */

#include "main.h" 

char *programpath;

int semid;
int shmid;

int main(int argc,char **argv)
{

  if(argc < 3)
    {
      printf("Usage: %s programpath count\n", argv[0]);
      return EXIT_FAILURE;
    }

  programpath = argv[2];
  int processcount = atoi(argv[1]);
  
  if(processcount == 0)
    {
      printf("%s is not a valid number\n", argv[1]);
      return EXIT_FAILURE;
    }

  int count;
  
  key_t key = createkey(66);
  if(key == (key_t) -1)
    {
      printf("\nFTOK Error\n");
      return EXIT_FAILURE;
    }

  size_t boxsize = sizeof(osmp_mailbox_t);
  size_t messagessize = sizeof(osmp_message_t) * OSMP_MAX_SLOTS;
  size_t osmpsize = sizeof(osmp_info_t) + processcount * sizeof(pid_t);

  size_t smsize = (processcount +1) * boxsize + osmpsize + messagessize; //+1 Da eine Mailbox fuer freie Plaezte
  
  shmid = shmget(key,smsize,IPC_CREAT | 0666);
  if(shmid == -1)
    {
      printf("\nSHMGET: %s\n",strerror(errno));
      return EXIT_FAILURE;
    }

  semid = semget(key,0,IPC_PRIVATE);
  if(semid < 0)
    {
      semid = semget(key,2+(2*processcount),IPC_CREAT | 0640);
      if(semid < 0)
	{
	  printf("\nSEMGET2: %s\n",strerror(errno));
	  return saveexit();
	}
      if(semctl(semid,0,SETVAL,(int)1) == -1)
	{
	  printf("\nSEMCTL1: %s\n",strerror(errno));
	  return saveexit();
	}
      if(semctl(semid,1,SETVAL,(int)OSMP_MAX_SLOTS) == -1)
	{
	  printf("\nSEMCTL2: %s\n",strerror(errno));
	  return saveexit();
	}
      for(count = 0;count < processcount;count++)
	{
	  if(semctl(semid,count+2,SETVAL,(int)0) == -1)
	    {
	      printf("\nSEMCTL3: %s\n",strerror(errno));
	      return saveexit();
	    }
	  if(semctl(semid,count+2+processcount,SETVAL,(int)OSMP_MAX_MESSAGES_PROC) == -1)
	    {
	      printf("\nSEMCTL4: %s\n",strerror(errno));
	      return saveexit();
	    }
	}
    }
  else
    {
      printf("\nID: %d SEMGET1: %s\n",semid,strerror(errno));
      return saveexit();
    }
  
  struct shmid_ds shmid_struct, *buf;
  buf = &shmid_struct;
  
  void* shm = shmat(shmid,NULL,0);
  if(shm == (void*)-1)
    {
      printf("\nSHMAT: %s\n",strerror(errno));
      return saveexit();
    }

  //Erstellen der OSMP_Info
  struct osmp_info *osinfo;
  osinfo = malloc(osmpsize);
  
  osinfo->processcount = processcount;
  osinfo->offset = osmpsize;

  //Erstellen der Mailboxen
  //Erste Mailbox fuer freie Nachrichten
  for(count = 0;count < (processcount+1);count++)
    {
      struct osmp_mailbox *box = calloc(1,boxsize);
      if(count == 0)
	{
	  box->first = 0;
	  box->last = OSMP_MAX_SLOTS;
	}
      else
	{
	  box->first = -1;
	  box->last = -1;
	}
      
      memcpy((((char*)shm)+osmpsize+count*boxsize),box,boxsize);
      
      free(box);
    }

  size_t messagesize = sizeof(osmp_message_t);
  
  //Erstellen der leeren Nachrichten
  for(count = 0;count < OSMP_MAX_SLOTS;count++)
    {
      struct osmp_message *msg = calloc(1,messagesize);
      msg->source = -1;
      msg->length = -1;
      msg->next = -1;
      memcpy((((char*)shm)+osmpsize+(processcount+1)*boxsize+count*messagesize),msg,messagesize);
      free(msg);
    }

  static struct sembuf sema;
  sema.sem_num = 0;
  sema.sem_op = -1;
  sema.sem_flg = SEM_UNDO;

  if(semop(semid,&sema,1) == -1)
    {
      free(osinfo);
      return saveexit();
    }
  
  printf("Start MUTEX: %d\n",semctl(semid,0,GETVAL));

  //Starten der Kindprozesse
  for(count = 0; count < processcount; count++)
    {
      pid_t newpid = fork();

      if(newpid == -1)
	{
	  printf("\n%s\n", strerror(errno));
	  return EXIT_FAILURE;
	}
      else if(newpid == 0)
	{
	  //Kindprozess
	  return childroutine(argv+3);
	}
      else
	{
	  printf("--- Elternprozess ---\n");
	  printf("Add pid: %d\n",newpid);
	  osinfo->pids[count]=newpid;
	  int x = listadd(newpid);
	  if(x != 0)
	    {
	      //Element konnte nicht hinzugefuegt werden.
	      return EXIT_FAILURE;
	    }
	}
    }

  memcpy(shm,osinfo,osmpsize);

  sema.sem_num = 0;
  sema.sem_op = 1;
  sema.sem_flg = SEM_UNDO;
  if(semop(semid,&sema,1) == -1)
    {
      free(osinfo);
      saveexit();
    }

  printf("Start MUTEX: %d\n",semctl(semid,0,GETVAL));

  printf("Warten auf Kinder\n");
  
  free(osinfo);
  
  while(listcount() != 0)
    {
      pid_t first = listgetfirst();
      if(first == -1)
	break;

      printf("--- Elternprozess ---\n");
      printf("Wait for pid: %d\n",first);
      
      pid_t error = waitpid(first,NULL,0);
      if(error == -1)
	{
	  printf("\n%s\n",strerror(errno));
	  saveexit();
	}
      listdelete(first);
    }

  int ret = semctl(semid,0,IPC_RMID,0);
  
  ret = shmctl(shmid,IPC_RMID,buf);
  if(ret == -1)
    {
      printf("\nSHMCTL_Remove: %s\n",strerror(errno));
      return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}

int childroutine(char **argv)
{
  printf("--- Kindprozess ---\n");
  
  int execerr = execv (programpath, argv);
  if(execerr == -1)
    {
      printf("Error: %s\n",strerror(errno));
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

int saveexit(void)
{
  int ret;
  
  if(semid>0)
    {
      ret = semctl(semid,0,IPC_RMID,0);
    }
  struct shmid_ds shmid_struct, *buf;
  buf = &shmid_struct;
  if(shmid > 0)
    {
      ret = shmctl(shmid,IPC_RMID,buf);
    }
  return EXIT_FAILURE;
}
