#include <string.h>
#include "insns.h"
#include "instruction.h"

// TODO: this is ineffecient, and worse, ugly
xo_instruction *xo_instruction_get(const char *name)
{
  for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    if(strcmp(name, xo_insns[i].name) == 0)
      return &xo_insns[i];
  return NULL;
}

void xo_instruction_print(FILE *file, const xo_instruction *insn, const char *suffix)
{
  fprintf(file, "%s/%zu%s", insn->name, insn->arity, suffix);
}
