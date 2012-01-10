#include "check.h"
#include "../src/parser.h"

int main()
{
  // nullary insns
  CHECK(xo_parser_validate("clc;"));
  CHECK(xo_parser_validate("cmc;"));
  CHECK(xo_parser_validate("stc;"));

  // unary insns
  CHECK(xo_parser_validate("dec r0;"));
  CHECK(xo_parser_validate("inc r0;"));
  CHECK(xo_parser_validate("neg r0;"));
  CHECK(xo_parser_validate("not r0;"));

  // binary insns
  CHECK(xo_parser_validate("adc r0,r1;"));
  CHECK(xo_parser_validate("add r0,r1;"));
  CHECK(xo_parser_validate("and r0,r1;"));
  CHECK(xo_parser_validate("cmova r0,r1;"));
  CHECK(xo_parser_validate("cmovbe r0,r1;"));
  CHECK(xo_parser_validate("cmovc r0,r1;"));
  CHECK(xo_parser_validate("cmovg r0,r1;"));
  CHECK(xo_parser_validate("cmovge r0,r1;"));
  CHECK(xo_parser_validate("cmovl r0,r1;"));
  CHECK(xo_parser_validate("cmovle r0,r1;"));
  CHECK(xo_parser_validate("cmovnc r0,r1;"));
  CHECK(xo_parser_validate("cmovno r0,r1;"));
  CHECK(xo_parser_validate("cmovnp r0,r1;"));
  CHECK(xo_parser_validate("cmovns r0,r1;"));
  CHECK(xo_parser_validate("cmovnz r0,r1;"));
  CHECK(xo_parser_validate("cmovo r0,r1;"));
  CHECK(xo_parser_validate("cmovp r0,r1;"));
  CHECK(xo_parser_validate("cmovs r0,r1;"));
  CHECK(xo_parser_validate("cmovz r0,r1;"));
  CHECK(xo_parser_validate("cmp r0,r1;"));
  CHECK(xo_parser_validate("mov r0,r1;"));
  CHECK(xo_parser_validate("or r0,r1;"));
  CHECK(xo_parser_validate("sbb r0,r1;"));
  CHECK(xo_parser_validate("sub r0,r1;"));
  CHECK(xo_parser_validate("xor r0,r1;"));

  // invalid insn
  CHECK(!xo_parser_validate("xxx;"));
  CHECK(!xo_parser_validate("xxx r0;"));
  CHECK(!xo_parser_validate("xxx r0,r1;"));

  // invalid arity
  CHECK(!xo_parser_validate("clc r0;"));
  CHECK(!xo_parser_validate("clc r0,r1;"));
  CHECK(!xo_parser_validate("dec;"));
  CHECK(!xo_parser_validate("dec r0,r1;"));
  CHECK(!xo_parser_validate("adc;"));
  CHECK(!xo_parser_validate("adc r0;"));

  return EXIT_SUCCESS;
}
