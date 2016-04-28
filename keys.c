#include "keys.h"

key_t createkey(int id)
{
  return ftok("/tmp",id);
}
  
