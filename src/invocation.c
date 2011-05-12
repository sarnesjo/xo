#include <stdio.h>
#include "instruction.h"
#include "invocation.h"

void xo_invocation_init(xo_invocation *inv, const char *insn_name, size_t r0, size_t r1)
{
  inv->insn = xo_instruction_get(insn_name);
  inv->r0 = r0;
  inv->r1 = r1;
}

void xo_invocation_invoke(const xo_invocation *inv, xo_machine_state *st)
{
  inv->insn->impl(st, inv->r0, inv->r1);
}

void xo_invocation_print(const xo_invocation *inv, const char *suffix)
{
  if(inv->r0 != XO_NODE_DEPENDENCY_NONE && inv->r1 != XO_NODE_DEPENDENCY_NONE)
    printf("%s %zu %zu%s", inv->insn->name, inv->r0, inv->r1, suffix);
  else if(inv->r0 != XO_NODE_DEPENDENCY_NONE)
    printf("%s %zu%s", inv->insn->name, inv->r0, suffix);
  else
    printf("%s%s", inv->insn->name, suffix);
}
