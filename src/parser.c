#include <stdio.h>
#include <string.h>
#include "parser.h"

// TODO: this should probably be moved elsewhere
static int arity_(const char *insn_name)
{
  if(strcmp(insn_name, "clc") == 0 || strcmp(insn_name, "cmc") == 0 || strcmp(insn_name, "stc") == 0)
    return 0;
  if(strcmp(insn_name, "dec") == 0 || strcmp(insn_name, "inc") == 0 || strcmp(insn_name, "not") == 0)
    return 1;
  if(strcmp(insn_name, "adc") == 0
      || strcmp(insn_name, "add") == 0
      || strcmp(insn_name, "and") == 0
      || strcmp(insn_name, "cmova") == 0
      || strcmp(insn_name, "cmovbe") == 0
      || strcmp(insn_name, "cmovc") == 0
      || strcmp(insn_name, "cmovg") == 0
      || strcmp(insn_name, "cmovge") == 0
      || strcmp(insn_name, "cmovl") == 0
      || strcmp(insn_name, "cmovle") == 0
      || strcmp(insn_name, "cmovnc") == 0
      || strcmp(insn_name, "cmovno") == 0
      || strcmp(insn_name, "cmovnp") == 0
      || strcmp(insn_name, "cmovns") == 0
      || strcmp(insn_name, "cmovnz") == 0
      || strcmp(insn_name, "cmovo") == 0
      || strcmp(insn_name, "cmovp") == 0
      || strcmp(insn_name, "cmovs") == 0
      || strcmp(insn_name, "cmovz") == 0
      || strcmp(insn_name, "cmp") == 0
      || strcmp(insn_name, "mov") == 0
      || strcmp(insn_name, "or") == 0
      || strcmp(insn_name, "sbb") == 0
      || strcmp(insn_name, "sub") == 0
      || strcmp(insn_name, "xor") == 0)
      return 2;
  return -1;
}

static bool insn_(const char *input, const char **remaining_input)
{
  char buf[8];
  int num_chars_consumed = 0;

  if(sscanf(input, " %7[a-z]%n", buf, &num_chars_consumed) != 1)
    return false;
  input += num_chars_consumed;

  int arity = arity_(buf);

  if(arity < 0)
    return false;

  if(arity >= 2)
  {
    if(sscanf(input, " r%1[0-7]%n", buf, &num_chars_consumed) != 1)
      return false;
    input += num_chars_consumed;

    if(sscanf(input, " %1[,]%n", buf, &num_chars_consumed) != 1)
      return false;
    input += num_chars_consumed;
  }

  if(arity >= 1)
  {
    if(sscanf(input, " r%1[0-7]%n", buf, &num_chars_consumed) != 1)
      return false;
    input += num_chars_consumed;
  }

  if(sscanf(input, " %1[;]%n", buf, &num_chars_consumed) != 1)
    return false;
  input += num_chars_consumed;

  if(remaining_input)
    *remaining_input = input;
  return true;
}

bool validate_and_count_insns_(const char *input, size_t *num_insns)
{
  if(!input)
    return false;

  size_t n = 0;

  // read all insns
  while(insn_(input, &input))
    ++n;

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
  return validate_and_count_insns_(input, NULL);
}

size_t xo_parser_count_insns(const char *input)
{
  size_t n = 0;
  validate_and_count_insns_(input, &n);
  return n;
}
