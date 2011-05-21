#include <stdio.h>
#include <stdlib.h>
#include "insns.h"
#include "invocation.h"
#include "parser.h"
#include "program.h"

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

static void did_parse_insn_(size_t i, const xo_instruction *insn, size_t r0, size_t r1, void *userdata)
{
  xo_program *prog = userdata;
  xo_invocation_init(&prog->invocations[i], insn, r0, r1);
}

xo_program *xo_program_create_from_str(const char *input)
{
  size_t num_invocations = xo_parser_count_insns(input);
  if(num_invocations == 0)
    return NULL;
  xo_program *prog = xo_program_create(num_invocations);
  xo_parser_traverse(input, did_parse_insn_, prog);
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
  }
}

void xo_program_print(const xo_program *prog, const char *suffix)
{
  for(size_t i = 0; i < prog->num_invocations; ++i)
    xo_invocation_print(&prog->invocations[i], " ");
  printf("%s", suffix);
}

void program_generate_(xo_program *prog, xo_program_callback callback, void *userdata, size_t inv)
{
  if(inv == prog->num_invocations)
  {
    callback(prog, userdata);
  }
  else
  {
    for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    {
      xo_instruction *insn = &xo_insns[i];

      switch(insn->arity)
      {
        case 0:
          xo_invocation_init(&prog->invocations[inv], insn, XO_REGISTER_NONE, XO_REGISTER_NONE);
          program_generate_(prog, callback, userdata, inv+1);
          break;
        case 1:
          for(size_t r0 = 0; r0 < XO_MACHINE_STATE_NUM_REGS; ++r0)
          {
            xo_invocation_init(&prog->invocations[inv], insn, r0, XO_REGISTER_NONE);
            program_generate_(prog, callback, userdata, inv+1);
          }
          break;
        case 2:
          for(size_t r0 = 0; r0 < XO_MACHINE_STATE_NUM_REGS; ++r0)
          {
            for(size_t r1 = 0; r1 < XO_MACHINE_STATE_NUM_REGS; ++r1)
            {
              xo_invocation_init(&prog->invocations[inv], insn, r0, r1);
              program_generate_(prog, callback, userdata, inv+1);
            }
          }
          break;
      }
    }
  }
}

void xo_program_generate(xo_program *prog, xo_program_callback callback, void *userdata)
{
  if(!prog)
    return;

  program_generate_(prog, callback, userdata, 0);
}
