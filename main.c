/* main.c */
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

#define  PROGRAMM "./writerr"

int main (int argc,char **argv) {
   pid_t pid;
   switch (pid = fork ()) {
   case -1:
     printf("\n%s\n",strerror(errno));
      return EXIT_FAILURE;
      break;
   case 0:
      sleep (5);   /* Kurze Pause */
      printf ("--- Im Kindprozess ---\n");
      int execerr = execlp (PROGRAMM, PROGRAMM, "Hallo", "Welt", NULL);
      if(execerr == -1)
      {
	printf("Error: %s\n",strerror(errno));
	return EXIT_FAILURE;
      }
      sleep(10); /*Wird nie erreicht wenn execlp ausgefuehrt wird*/
      printf ("--- Kindprozess Ende ---\n");
      break;
   default:
      printf ("--- Im Elternprozess ---\n");
      printf ("--- Warte auf Kind %d ---\n",pid);
      pid_t error = waitpid(pid,NULL,0);
      if(error == -1)
      {
	printf("\n%s\n",strerror(errno));
	return EXIT_FAILURE;
      }
      printf("--- Elternprozess Ende ---\n");
      break;
   }
   return EXIT_SUCCESS;
}
