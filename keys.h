#ifndef KEYS_H_
#define KEYS_H_

#include <sys/ipc.h>
#include <sys/types.h>

int createkey(int id);
key_t getkey();

#endif
