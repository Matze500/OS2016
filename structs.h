#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <sys/types.h>

typedef struct osmp_info
{
  int processcount;
  size_t offset;
  pid_t pids[];
} osmp_info_t;

#endif
