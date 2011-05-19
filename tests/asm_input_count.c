#include "check.h"
#include "../src/parser.h"

int main()
{
  // valid insns
  CHECK(xo_parser_count_insns("clc;") == 1);
  CHECK(xo_parser_count_insns("dec r0;") == 1);
  CHECK(xo_parser_count_insns("adc r0,r1;") == 1);

  // multiple valid insns
  CHECK(xo_parser_count_insns("clc;clc;") == 2);
  CHECK(xo_parser_count_insns("dec r0;dec r0;") == 2);
  CHECK(xo_parser_count_insns("adc r0,r1;adc r0,r1;") == 2);

  // invalid insns
  CHECK(xo_parser_count_insns("xxx;") == 0);
  CHECK(xo_parser_count_insns("xxx r0;") == 0);
  CHECK(xo_parser_count_insns("xxx r0,r1;") == 0);

  // invalid arity
  CHECK(xo_parser_count_insns("clc r0;") == 0);
  CHECK(xo_parser_count_insns("clc r0,r1;") == 0);
  CHECK(xo_parser_count_insns("dec;") == 0);
  CHECK(xo_parser_count_insns("dec r0,r1;") == 0);
  CHECK(xo_parser_count_insns("adc;") == 0);
  CHECK(xo_parser_count_insns("adc r0;") == 0);

  // mixed valid/invalid insns
  CHECK(xo_parser_count_insns("clc;clc r0;") == 0);
  CHECK(xo_parser_count_insns("dec r0;dec;") == 0);
  CHECK(xo_parser_count_insns("adc r0,r1;adc;") == 0);

  return EXIT_SUCCESS;
}
