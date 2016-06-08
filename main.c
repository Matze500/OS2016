/* main.c */

#include "main.h" 

char* programpath = "";

int main(int argc,char **argv)
{

  if(argc != 3)
    {
      printf("Usage: %s programpath count\n", argv[0]);
      return EXIT_FAILURE;
    }

  programpath = argv[1];
  int processcount = atoi(argv[2]);
  
  if(processcount == 0)
    {
      printf("%s is not a valid number\n", argv[1]);
      return EXIT_FAILURE;
    }

  int count;
  
  key_t key = createkey(666);
  if(key == (key_t) -1)
    {
      printf("\nFTOK Error\n");
      return EXIT_FAILURE;
    }

  size_t boxsize = sizeof(osmp_mailbox_t);
  uint8_t messagearr[256];
  size_t osmpsize = sizeof(osmp_info_t) + sizeof(messagearr)+ processcount * sizeof(pid_t);

  size_t smsize = processcount * boxsize + osmpsize;
  
  int shmid = shmget(key,smsize,IPC_CREAT | 0666);
  if(shmid == -1)
    {
      printf("\nSHMGET: %s\n",strerror(errno));
      return EXIT_FAILURE;
    }

  int semid = semget(key,0,IPC_PRIVATE);
  if(semid < 0)
    {
      semid = semget(key,2+processcount,IPC_CREAT | 0640);
      if(semid < 0)
	{
	  printf("\nSEMGET: %s\n",strerror(errno));
	  return EXIT_FAILURE;
	}
      if(semctl(semid,0,SETVAL,(int)1) == -1)
	{
	  printf("\nSEMCTL1: %s\n",strerror(errno));
	  return EXIT_FAILURE;
	}
      if(semctl(semid,1,SETVAL,(int)256) == -1)
	{
	  printf("\nSEMCTL2: %s\n",strerror(errno));
	  return EXIT_FAILURE;
	}
      for(count = 0;count < processcount;count++)
	{
	  if(semctl(semid,count+2,SETVAL,(int)16) == -1)
	    {
	      printf("\nSEMCTL3: %s\n",strerror(errno));
	      return EXIT_FAILURE;
	    }
	}
    }
  else
    {
      printf("\nSEMGET: %s\n",strerror(errno));
      return EXIT_FAILURE;
    }
  
  struct shmid_ds shmid_struct, *buf;
  buf = &shmid_struct;
  
  void* shm = shmat(shmid,NULL,0);
  if(shm == (void*)-1)
    {
      printf("\nSHMAT: %s\n",strerror(errno));
      return EXIT_FAILURE;
    }

  //Erstellen der OSMP_Info
  
  memset(messagearr,1,sizeof(messagearr));
  
  struct osmp_info *osinfo;
  osinfo = malloc(osmpsize);
  
  osinfo->processcount = processcount;
  osinfo->offset = osmpsize;
  
  memcpy(osinfo->messages, messagearr, sizeof(messagearr));


  //Erstellen der Messageboxen
  
  for(count = 0;count < processcount;count++)
    {
      struct osmp_mailbox *box = NULL;
      box = malloc(boxsize);
      memcpy((shm+osmpsize+count*boxsize),box,boxsize);
    }


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
	  return childroutine();
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
  
  while(listcount() != 0)
    {
      pid_t first = listgetfirst();
      if(first == -1)
	break;

      printf("--- Elternprozess ---\n");
      printf("Wait for pid: %d\n",first);
      
      pid_t error = waitpid(first,NULL,0); // alleine übe wait...
      if(error == -1)
	{
	  printf("\n%s\n",strerror(errno));
	  return EXIT_FAILURE;
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

int childroutine()
{
  printf("--- Kindprozess ---\n");
  
  int execerr = execlp (programpath, programpath, "Hallo", "Welt", NULL);
  if(execerr == -1)
    {
      printf("Error: %s\n",strerror(errno));
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
