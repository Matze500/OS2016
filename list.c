/*list.c*/
#include "list.h"

struct pidlist
{
  pid_t pid;
  struct pidlist *next;
};

struct pidlist *start = NULL;
struct pidlist *next = NULL;

int listadd(pid_t key)
{
  struct pidlist *zeiger;

  if(start == NULL)
    {
      if((start = malloc(sizeof(struct pidlist))) == NULL)
	{
	  //Kein Spiecher da
	  return -1;
	}
      start->pid = key;
      start->next =NULL;
    }
  else
    {
      zeiger = start;
      while(zeiger->next != NULL)
	zeiger = zeiger->next;

      if((zeiger->next = malloc(sizeof(struct pidlist))) == NULL)
	{
	  //Kein Speicher da
	  return -1;
	}
      zeiger = zeiger->next;
      zeiger->pid = key;
      zeiger->next = NULL;
    }
  return 0;
}
int listdelete(pid_t key)
{
  struct pidlist *zeiger, *zeiger1;

  if(start != NULL)
    {
      if(key == start->pid)
	{
	  zeiger = start->next;
	  free(start);
	  start = zeiger;
	}
      else
	{
	  zeiger = start;
	  while(zeiger->next != NULL)
	    {
	      zeiger1 = zeiger->next;
	      if(key == zeiger->pid)
		{
		  zeiger->next = zeiger1->next;
		  free(zeiger1);
		  break;
		}
	      zeiger = zeiger1;
	    }
	}
    }
  else
    {
      return -1;
    }
  return 0;
}

int listcount()
{
  struct pidlist *zeiger;
  int count = 1;
  
  if(start == NULL)
    {
      return 0;
    }
  else
    {
      zeiger = start;
      while(zeiger->next != NULL)
	{
	  zeiger = zeiger->next;
	  count++;
	}
      return count;
    }
}

pid_t listgetfirst()
{
  if(start == NULL)
    {
      return -1;
    }
  else
    {
      return start->pid;
    }
}
