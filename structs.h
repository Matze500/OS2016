#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <sys/types.h>


typedef struct osmp_info
{
  int processcount;
  size_t offset;
  uint8_t messages[256];
  pid_t pids[];
} osmp_info_t;


typedef struct osmp_message
{
  int source;
  int dest;
  int length;
  char data[128];
} osmp_message_t;

typedef struct osmp_mailbox
{
  int first;
  int last;
  osmp_message_t *mailbox[16];
  } osmp_mailbox_t;

#endif
