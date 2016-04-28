/* writer.c */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "keys.h"

int main(int argc, char **argv) {
  sleep(5);
   int i;
   printf("Progammname : %s\n", argv[0]);   
   for( i=1; i < argc; i++)
      printf("Argumente %d : %s\n", i, argv[i]);

   OSMP_Init(NULL,&argv);
   OSMP_Size(NULL);
   OSMP_Rank(NULL);
   OSMP_Send(NULL,0,0);
   OSMP_Recv(NULL,0,NULL,NULL);
   OSMP_Finalize();

   key_t key = createkey(142);
   if(key == (key_t)-1)
     {
       printf("\nFTOK Error\n");
       return EXIT_FAILURE;
     }
   int shmid = shmget(key,1024,0666);
   if(shmid == -1)
     {
       printf("\nSHMGET Error\n");
       return EXIT_FAILURE;
     }
   void *shm = shmat(shmid,NULL,0);
   if(shm == (void*)-1)
     {
       printf("\nSHMAT Error\n");
       return EXIT_FAILURE;
     }
   /*char *s;

   for(s = (char*)shm; *s != NULL; s++)
     putchar(*s);
     putchar('\n');*/

   return EXIT_SUCCESS;
}
