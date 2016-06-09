/* writer.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "OSMP.h"

int main(int argc, char **argv) {
   int i;   
   for( i=0; i < argc; i++)
      printf("Argumente %d : %s\n", i, argv[i]);

   int size = 0;
   int rank = 0;

   
   if(OSMP_Init(&argc,&argv) == -1)
     {
       printf("Init: OSMP_ERROR\n");
       return EXIT_FAILURE;
     }
   if(OSMP_Size(&size) == -1)
     {
       printf("Size: OSMP_ERROR\n");
       return EXIT_FAILURE;
     }
   printf("Size: %d\n",size);
   if(OSMP_Rank(&rank) == -1)
     {
       printf("Rank: OSMP_ERROR\n");
       return EXIT_FAILURE;
     }
   printf("Rank: %d\n",rank);
 
   if(rank == 0)
     {
       int *bufin = malloc(2*sizeof(int));
       bufin[0] = 1;
       bufin[1] = 2;

       if(OSMP_Send(bufin,sizeof(bufin),1) == -1)
	 {
	   printf("Send: OSMP_ERROR\n");
	   free(bufin);
	   return EXIT_FAILURE;
	 }
	free(bufin);
     }
   else
     {
       int *bufout = malloc(2*sizeof(int));
       int source;
       int len;
       
       if(OSMP_Recv(bufout,sizeof(bufout),&source,&len) == -1)
	 {
	   printf("Recv: OSMP_ERROR\n");
	   free(bufout);
	   return EXIT_FAILURE;
	 }
       printf("Process %d received %d byte from %d [%d:%d]\n",rank,len,source,bufout[0],bufout[1]);
       free(bufout);
     }
   
   if(OSMP_Finalize() == -1)
     {
       printf("Finalize: OSMP_ERROR\n");
       return EXIT_FAILURE;
     }

   return EXIT_SUCCESS;
}
