#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// any register read from before written to is an input register
// the last register written to is the output register
// TODO: can't use uint8_t if XO_MACHINE_STATE_NUM_REGS > 8
void xo_program_analyze(const xo_program *prog, uint8_t *input_regs, uint8_t *output_regs)
{
  uint8_t iregs = 0, oreg = 0, written_regs = 0;

  for(size_t i = 0; i < prog->num_invocations; ++i)
  {
    xo_invocation *inv = &prog->invocations[i];
    const char *insn_name = inv->insn->name;
    size_t r0 = inv->r0;
    size_t r1 = inv->r1;

    // TODO: this information should be stored in instruction
    bool no_i = (strcmp(insn_name, "sbb") == 0 || strcmp(insn_name, "sub") == 0 || strcmp(insn_name, "xor") == 0) && (r0 == r1);
    bool no_o = strcmp(insn_name, "cmp") == 0;
    bool o_not_i = strcmp(insn_name, "mov") == 0;

    if(r0 != XO_REGISTER_NONE && ((written_regs & (1 << r0)) == 0) && !no_i && !o_not_i)
      iregs |= 1 << r0;
    if(r1 != XO_REGISTER_NONE && ((written_regs & (1 << r1)) == 0) && !no_i)
      iregs |= 1 << r1;

    if(r0 != XO_REGISTER_NONE && !no_o)
    {
      oreg = 1 << r0; // only one output register supported
      written_regs |= 1 << r0;
    }
  }

  if(input_regs)
    *input_regs = iregs;

  if(output_regs)
    *output_regs = oreg;
}
