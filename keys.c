#include "keys.h"

key_t smkey = 0;

int createkey(int id)
{
  if((smkey = ftok("/tmp",id)) == (key_t) -1)
    {
      return -1;
    }
  return 0;
}

key_t getkey()
{
  return smkey;
}
  
