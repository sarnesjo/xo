#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "insns.h"
#include "invocation.h"
#include "machine_state.h"
#include "node.h"
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

// --

void xo_program_generate_from_graph_(xo_program *prog, const xo_graph *graph, xo_program_callback callback, size_t i)
{
  assert(prog->num_invocations + NUM_INPUTS == graph->num_nodes); // TODO: let this function create program internally instead?
  xo_node *node = &graph->nodes[NUM_INPUTS+i];

  size_t num_insns = 0;
  xo_instruction *insns = NULL;

  switch(xo_node_arity(node))
  {
    case 2:
      num_insns = XO_NUM_INSNS2;
      insns = xo_insns2;
      break;
    case 1:
      num_insns = XO_NUM_INSNS1;
      insns = xo_insns1;
      break;
    case 0:
      num_insns = XO_NUM_INSNS0;
      insns = xo_insns0;
      break;
  }

  // TODO: should generate both d0,d1 and d1,d0 for noncomm insns
  // TODO: should not generate cmov insns when flag is undefined
  // TODO: should not generate mov/cmov insns that copies register to itself... how to model?

  for(size_t ins = 0; ins < num_insns; ++ins)
  {
    xo_invocation_init(&prog->invocations[i], insns[ins].name, node->dependency0, node->dependency1);
    if(i == prog->num_invocations-1)
      callback(prog);
    else
      xo_program_generate_from_graph_(prog, graph, callback, i+1);
  }
}

void xo_program_generate_from_graph(xo_program *prog, const xo_graph *graph, xo_program_callback callback)
{
  xo_program_generate_from_graph_(prog, graph, callback, 0);
}
