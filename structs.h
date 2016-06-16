#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <sys/types.h>

#define OSMP_MAX_MESSAGES_PROC 16
#define OSMP_MAX_SLOTS 256
#define OSMP_MAX_PAYLOAD_LENGTH 128

typedef struct osmp_info
{
  int processcount;
  size_t offset;
  pid_t pids[];
} osmp_info_t;


typedef struct osmp_message
{
  int source;
  int length;
  char data[OSMP_MAX_PAYLOAD_LENGTH];
  int next;
} osmp_message_t;

typedef struct osmp_mailbox //pro Prozess + fuer freie Nachrichten
{
  int first;
  int last;
  } osmp_mailbox_t;

#endif
