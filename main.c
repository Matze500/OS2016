/* main.c */
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define  PROGRAMM "./writer"




int main (void) {
   pid_t pid;
   switch (pid = fork ()) {
   case -1:
      printf ("Fehler bei fork()\n");
      break;
   case 0:
      sleep (5);   /* Kurze Pause */
      printf ("--- Im Kindprozess ---\n");
      execlp (PROGRAMM, PROGRAMM, "Hallo", "Welt", NULL);
      sleep(10); /*Wird nie erreicht wenn execlp ausgefuehrt wird*/
      printf ("--- Kindprozess Ende ---\n");
      break;
   default:
      printf ("--- Im Elternprozess ---\n");
      printf ("--- Warte auf Kind %d ---\n",pid);
      waitpid(pid);
      printf("--- Elternprozess Ende ---\n");
      break;
   }
   return EXIT_SUCCESS;
}
