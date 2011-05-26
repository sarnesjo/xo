#include <stdlib.h>
#include <string.h>
#include "invocation.h"
#include "machine_state.h"
#include "parser.h"
#include "program.h"
#include "register_set.h"

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

// any register read from before written to is an input register
// the last register written to is the output register
void xo_program_analyze(const xo_program *prog, xo_register_set *input_regs, xo_register_set *output_regs)
{
  xo_register_set iregs = 0, oreg = 0, written_regs = 0;

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

bool xo_program_equal(const xo_program *prog1, const xo_program *prog2)
{
  if(prog1->num_invocations != prog2->num_invocations)
    return false;
  for(size_t i = 0; i < prog1->num_invocations; ++i)
    if(!xo_invocation_equal(&prog1->invocations[i], &prog2->invocations[i]))
      return false;
  return true;
}

bool xo_program_equivalent_on_states(const xo_program *prog1, const xo_program *prog2, size_t num_states, const xo_machine_state *states)
{
  xo_register_set output_reg_set_1, output_reg_set_2;

  xo_program_analyze(prog1, NULL, &output_reg_set_1);
  xo_program_analyze(prog2, NULL, &output_reg_set_2);

  if(output_reg_set_1 != output_reg_set_2)
    return false; // TODO: signal error?

  size_t output_reg_index = xo_register_set_first_live_index(output_reg_set_1);

  for(size_t i = 0; i < num_states; ++i)
  {
    uint32_t v1 = xo_program_return_value_for_state(prog1, &states[i], output_reg_index);
    uint32_t v2 = xo_program_return_value_for_state(prog2, &states[i], output_reg_index);

    if(v1 != v2)
      return false;
  }
  return true;
}

void xo_program_run_on_state(const xo_program *prog, xo_machine_state *st)
{
  for(size_t i = 0; i < prog->num_invocations; ++i)
  {
    xo_invocation *inv = &prog->invocations[i];
    xo_invocation_invoke(inv, st);
  }
}

uint32_t xo_program_return_value_for_state(const xo_program *prog, const xo_machine_state *st, size_t output_reg_index)
{
  xo_machine_state st_copy;
  xo_machine_state_copy(&st_copy, st);
  xo_program_run_on_state(prog, &st_copy);
  return st_copy.regs[output_reg_index];
}

void xo_program_print(FILE *file, const xo_program *prog, const char *suffix)
{
  for(size_t i = 0; i < prog->num_invocations; ++i)
    xo_invocation_print(file, &prog->invocations[i], " ");
  fprintf(file, "%s", suffix);
}
