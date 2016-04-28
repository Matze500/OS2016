/* writer.c */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
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
   
   return EXIT_SUCCESS;
}
