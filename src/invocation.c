#include "invocation.h"

void xo_invocation_init(xo_invocation *inv, const xo_instruction *insn, size_t r0, size_t r1)
{
  inv->insn = insn;
  inv->r0 = r0;
  inv->r1 = r1;
}

void xo_invocation_invoke(const xo_invocation *inv, xo_machine_state *st)
{
  inv->insn->impl(st, inv->r0, inv->r1);
}

void xo_invocation_print(FILE *file, const xo_invocation *inv, const char *suffix)
{
  if(inv->r0 != XO_REGISTER_NONE && inv->r1 != XO_REGISTER_NONE)
    fprintf(file, "%s r%zu,r%zu;%s", inv->insn->name, inv->r0, inv->r1, suffix);
  else if(inv->r0 != XO_REGISTER_NONE)
    fprintf(file, "%s r%zu;%s", inv->insn->name, inv->r0, suffix);
  else
    fprintf(file, "%s;%s", inv->insn->name, suffix);
}
