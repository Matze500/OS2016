#ifndef MAIN_H_
#define MAIN_H_

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "list.h"
#include "keys.h"

int main(int argc,char **argv);
int childroutine();

#endif
