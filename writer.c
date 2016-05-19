/* writer.c */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include "keys.h"

int main(int argc, char **argv) {
  sleep(5);
   int i;
   printf("Progammname : %s\n", argv[0]);   
   for( i=1; i < argc; i++)
      printf("Argumente %d : %s\n", i, argv[i]);

   int size = 0;
   int rank = 0;

   
   OSMP_Init(NULL,&argv);
   OSMP_Size(&size);
   printf("Size: %d\n",size);
   OSMP_Rank(&rank);
   printf("Rank: %d\n",rank);
   OSMP_Send(NULL,0,0);
   OSMP_Recv(NULL,0,NULL,NULL);
   
   OSMP_Finalize();

   return EXIT_SUCCESS;
}
