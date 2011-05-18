#include <stdio.h>
#include "parser.h"

static bool insn_(const char *input, const char **remaining_input)
{
  char buf[8];
  int num_chars_consumed = 0;

  if(sscanf(input, " %7[a-z]%n", buf, &num_chars_consumed) != 1)
    return false;
  input += num_chars_consumed;
  if(sscanf(input, " %7[a-z]%n", buf, &num_chars_consumed) != 1)
    return false;
  input += num_chars_consumed;
  if(sscanf(input, " %1[,]%n", buf, &num_chars_consumed) != 1)
    return false;
  input += num_chars_consumed;
  if(sscanf(input, " %7[a-z]%n", buf, &num_chars_consumed) != 1)
    return false;
  input += num_chars_consumed;
  if(sscanf(input, " %1[;]%n", buf, &num_chars_consumed) != 1)
    return false;
  input += num_chars_consumed;

  if(remaining_input)
    *remaining_input = input;
  return true;
}

bool xo_parser_validate(const char *input)
{
  if(!input)
    return false;

  // read all insns
  while(insn_(input, &input));

  // make sure nothing but whitespace remains (ugly)
  while(*input)
  {
    if(*input != ' ')
      return false;
    ++input;
  }

  return true;
}
