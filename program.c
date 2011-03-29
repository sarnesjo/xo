#include <stdio.h>
#include <stdlib.h>
#include "invocation.h"
#include "machine_state.h"
#include "program.h"
#include "xo.h"

xo_program *xo_program_create(size_t num_invocations)
{
  xo_program *prog = malloc(sizeof(xo_program));
  if(prog)
  {
    prog->num_invocations = num_invocations;
    prog->invocations = malloc(num_invocations * sizeof(xo_invocation)); // TODO: check this malloc
  }
  return prog;
}

void xo_program_destroy(xo_program *prog)
{
  free(prog->invocations);
  free(prog);
}

void xo_program_run(const xo_program *prog, xo_machine_state *st)
{
  for(size_t i = 0; i < prog->num_invocations; ++i)
  {
    xo_invocation *inv = &prog->invocations[i];
    xo_invocation_invoke(inv, st);
    if(VERBOSITY > 0)
    {
      xo_invocation_print(inv, ": ");
      xo_machine_state_print(st, "\n");
    }
  }
}

void xo_program_print(const xo_program *prog, const char *suffix)
{
  for(size_t i = 0; i < prog->num_invocations; ++i)
    xo_invocation_print(&prog->invocations[i], "\n");
  printf("%s", suffix);
}
