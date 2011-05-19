#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "types.h"

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

static bool insn_(const char *input, const char **remaining_input, char out_insn_name[8], size_t *out_r0, size_t *out_r1)
{
  char buf[8], insn_name[8]; // TODO: define max insn name length (to 7) somewhere
  size_t r0 = XO_NODE_DEPENDENCY_NONE, r1 = XO_NODE_DEPENDENCY_NONE;
  int num_chars_consumed = 0;

  if(sscanf(input, " %7[a-z]%n", buf, &num_chars_consumed) != 1)
    return false;
  strlcpy(insn_name, buf, 8);
  input += num_chars_consumed;

  int arity = arity_(buf);

  if(arity < 0)
    return false;

  if(arity >= 1)
  {
    if(sscanf(input, " r%1[0-7]%n", buf, &num_chars_consumed) != 1)
      return false;
    r0 = strtol(buf, NULL, 0);
    input += num_chars_consumed;
  }

  if(arity >= 2)
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
  if(out_insn_name)
    strlcpy(out_insn_name, insn_name, 8);
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

  char insn_name[8];
  size_t r0, r1;
  size_t n = 0;

  // read all insns
  while(insn_(input, &input, insn_name, &r0, &r1))
  {
    if(callback)
      callback(n, insn_name, r0, r1, userdata);
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
