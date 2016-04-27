#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

extern int listdelete(pid_t key);
extern int listadd(pid_t key);
extern pid_t listgetfirst();
extern int listcount();

#endif
