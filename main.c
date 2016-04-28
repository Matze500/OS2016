/* main.c */

#include "main.h"

#define  PROGRAMM "./osmpexecutable"

int main (int argc,char **argv) {

  if(argc != 2)
    {
      printf("Usage: %s count\n", argv[0]);
      return EXIT_FAILURE;
    }

  int processcount = atoi(argv[1]);

  if(processcount == 0)
    {
      printf("%s is not a valid number\n", argv[1]);
      return EXIT_FAILURE;
    }

  createkey(42);

  key_t key = getkey();
  printf("Shardmemkey: %d\n",(int)key);
  int shmg = shmget(key,1024,IPC_CREAT);
  printf("Shmget: %d\n",shmg);
  char* shm = shmat(shmg,NULL,0);
  if(shm == (char*)-1)
    {
      printf("\n%s\n",strerror(errno));
    }
  
  int i;
  
  for(i = 0; i < processcount; i++)
    {
      sleep(3);
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
	  int x = listadd(newpid);
	  if(x != 0)
	    {
	      //Element konnte nicht hinzugefuegt werden.
	      return EXIT_FAILURE;
	    }
	}
    }

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
	  return EXIT_FAILURE;
	}
      listdelete(first);
    }
  
  return EXIT_SUCCESS;
}

int childroutine()
{
  sleep(5);
  printf("--- Kindprozess ---\n");
  int execerr = execlp (PROGRAMM, PROGRAMM, "Hallo", "Welt", NULL);
  if(execerr == -1)
    {
      printf("Error: %s\n",strerror(errno));
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
