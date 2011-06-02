#include <stdlib.h>
#include "insns.h"
#include "invocation.h"
#include "machine_state.h"
#include "parser.h"
#include "program.h"
#include "register_set.h"

xo_program *xo_program_create(size_t num_invocations)
{
  xo_program *prog = malloc(sizeof(xo_program));

  if(!prog)
    return NULL;

  prog->num_invocations = num_invocations;
  prog->invocations = malloc(num_invocations * sizeof(xo_invocation));

  if(!prog->invocations)
  {
    free(prog);
    return NULL;
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
  if(!prog)
    return;

  free(prog->invocations);
  free(prog);
}

// any register read from before written to is an input register
// the last register written to is the output register
void xo_program_analyze(const xo_program *prog, xo_register_set *input_regs, xo_register_set *output_regs)
{
  xo_register_set iregs = 0, oreg = 0, written_regs = 0;

  for(size_t i = 0; i < prog->num_invocations; ++i)
  {
    xo_invocation *inv = &prog->invocations[i];
    const xo_instruction *insn = inv->insn;
    size_t r0 = inv->r0;
    size_t r1 = inv->r1;

    // TODO: this information should be stored in instruction
    bool no_i = (insn->id == XO_INSN_SUB || insn->id == XO_INSN_SBB || insn->id == XO_INSN_XOR) && (r0 == r1);
    bool no_o = insn->id == XO_INSN_CMP;
    bool o_not_i = insn->id == XO_INSN_MOV;

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

bool xo_program_equal(const xo_program *prog1, const xo_program *prog2)
{
  if(prog1->num_invocations != prog2->num_invocations)
    return false;
  for(size_t i = 0; i < prog1->num_invocations; ++i)
    if(!xo_invocation_equal(&prog1->invocations[i], &prog2->invocations[i]))
      return false;
  return true;
}

void xo_program_print(FILE *file, const xo_program *prog, const char *suffix)
{
  for(size_t i = 0; i < prog->num_invocations; ++i)
    xo_invocation_print(file, &prog->invocations[i], " ");
  fprintf(file, "%s", suffix);
}
