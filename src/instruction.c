#include <string.h>
#include "insns.h"
#include "instruction.h"

// TODO: this is ineffecient, and worse, ugly
xo_instruction *xo_instruction_get(const char *name)
{
  for(size_t i = 0; i < XO_NUM_INSNS0; ++i)
    if(strcmp(name, xo_insns0[i].name) == 0)
      return &xo_insns0[i];
  for(size_t i = 0; i < XO_NUM_INSNS1; ++i)
    if(strcmp(name, xo_insns1[i].name) == 0)
      return &xo_insns1[i];
  for(size_t i = 0; i < XO_NUM_INSNS2; ++i)
    if(strcmp(name, xo_insns2[i].name) == 0)
      return &xo_insns2[i];
  return NULL;
}
