/* main.c */

#include "main.h"

#define  PROGRAMM "./osmpexecutable"
// Ueber argumentliste... 

char* programpath;

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
  
  key_t key = createkey(666);
  if(key == (key_t) -1)
    {
      printf("\nFTOK Error\n");
      return EXIT_FAILURE;
    }
  
  int shmid = shmget(key,1024,IPC_CREAT | 0666);
  if(shmid == -1)
    {
      printf("\nSHMGET: %s\n",strerror(errno));
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

  struct osmp_info *osinfo;
  size_t offset = sizeof(osmp_info_t) + processcount * sizeof(pid_t);
  osinfo = malloc(offset);
  
  osinfo->processcount = processcount;
  osinfo->offset = offset;
 
  int i;
  
  for(i = 0; i < processcount; i++)
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
	  osinfo->pids[i]=newpid;
	  int x = listadd(newpid);
	  if(x != 0)
	    {
	      //Element konnte nicht hinzugefuegt werden.
	      return EXIT_FAILURE;
	    }
	}
    }

  memcpy(shm,osinfo,offset);
  
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

  int ret = shmctl(shmid,IPC_RMID,buf);
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
