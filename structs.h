#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <sys/types.h>

typedef struct osmp_info
{
  int processcount;
  size_t offset;
  pid_t pids[];
} osmp_info_t;


typedef struct osmp_mailbox
{
  int first;
  int last;
  uint16_t mailbox[16];
  } osmp_mailbox_t;

typedef struct osmp_message
{
  int source;
  int dest;
  size_t length;
  uint16_t message[128];
} osmp_message_t;

#endif
