#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "parser.h"

static bool insn_(const char *input, const char **remaining_input, xo_instruction **out_insn, size_t *out_r0, size_t *out_r1)
{
  char buf[8]; // TODO: define max insn name length (to 7) somewhere
  size_t r0 = XO_REGISTER_NONE, r1 = XO_REGISTER_NONE;
  int num_chars_consumed = 0;

  if(sscanf(input, " %7[a-z]%n", buf, &num_chars_consumed) != 1)
    return false;
  input += num_chars_consumed;

  xo_instruction *insn = xo_instruction_get(buf);

  if(!insn)
    return false;

  if(insn->arity >= 1)
  {
    if(sscanf(input, " r%1[0-7]%n", buf, &num_chars_consumed) != 1)
      return false;
    r0 = strtol(buf, NULL, 0);
    input += num_chars_consumed;
  }

  if(insn->arity >= 2)
  {
    if(sscanf(input, " %1[,]%n", buf, &num_chars_consumed) != 1)
      return false;
    input += num_chars_consumed;

    if(sscanf(input, " r%1[0-7]%n", buf, &num_chars_consumed) != 1)
      return false;
    r1 = strtol(buf, NULL, 0);
    input += num_chars_consumed;
  }

  if(sscanf(input, " %1[;]%n", buf, &num_chars_consumed) != 1)
    return false;
  input += num_chars_consumed;

  if(remaining_input)
    *remaining_input = input;
  if(out_insn)
    *out_insn = insn;
  if(out_r0)
    *out_r0 = r0;
  if(out_r1)
    *out_r1 = r1;

  return true;
}

bool validate_count_traverse_(const char *input, size_t *num_insns, xo_parser_traverse_callback callback, void *userdata)
{
  if(!input)
    return false;

  xo_instruction *insn = NULL;
  size_t r0 = XO_REGISTER_NONE, r1 = XO_REGISTER_NONE;
  size_t n = 0;

  // read all insns
  while(insn_(input, &input, &insn, &r0, &r1))
  {
    if(callback)
      callback(n, insn, r0, r1, userdata);
    ++n;
  }

  // make sure nothing but whitespace remains (ugly)
  while(*input)
  {
    if(*input != ' ')
      return false;
    ++input;
  }

  if(num_insns)
    *num_insns = n;
  return true;
}

bool xo_parser_validate(const char *input)
{
  return validate_count_traverse_(input, NULL, NULL, NULL);
}

size_t xo_parser_count_insns(const char *input)
{
  size_t n = 0;
  validate_count_traverse_(input, &n, NULL, NULL);
  return n;
}

void xo_parser_traverse(const char *input, xo_parser_traverse_callback callback, void *userdata)
{
  validate_count_traverse_(input, NULL, callback, userdata);
}
