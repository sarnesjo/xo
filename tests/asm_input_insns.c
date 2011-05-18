#include "check.h"
#include "../src/parser.h"

int main()
{
  // nullary insns
  CHECK(xo_parser_validate("clc;"));
  CHECK(xo_parser_validate("cmc;"));
  CHECK(xo_parser_validate("stc;"));

  // unary insns
  CHECK(xo_parser_validate("dec a;"));
  CHECK(xo_parser_validate("inc a;"));
  CHECK(xo_parser_validate("not a;"));

  // binary insns
  CHECK(xo_parser_validate("adc a,b;"));
  CHECK(xo_parser_validate("add a,b;"));
  CHECK(xo_parser_validate("and a,b;"));
  CHECK(xo_parser_validate("cmova a,b;"));
  CHECK(xo_parser_validate("cmovbe a,b;"));
  CHECK(xo_parser_validate("cmovc a,b;"));
  CHECK(xo_parser_validate("cmovg a,b;"));
  CHECK(xo_parser_validate("cmovge a,b;"));
  CHECK(xo_parser_validate("cmovl a,b;"));
  CHECK(xo_parser_validate("cmovle a,b;"));
  CHECK(xo_parser_validate("cmovnc a,b;"));
  CHECK(xo_parser_validate("cmovno a,b;"));
  CHECK(xo_parser_validate("cmovnp a,b;"));
  CHECK(xo_parser_validate("cmovns a,b;"));
  CHECK(xo_parser_validate("cmovnz a,b;"));
  CHECK(xo_parser_validate("cmovo a,b;"));
  CHECK(xo_parser_validate("cmovp a,b;"));
  CHECK(xo_parser_validate("cmovs a,b;"));
  CHECK(xo_parser_validate("cmovz a,b;"));
  CHECK(xo_parser_validate("cmp a,b;"));
  CHECK(xo_parser_validate("mov a,b;"));
  CHECK(xo_parser_validate("or a,b;"));
  CHECK(xo_parser_validate("sbb a,b;"));
  CHECK(xo_parser_validate("sub a,b;"));
  CHECK(xo_parser_validate("xor a,b;"));

  // invalid insn
  CHECK(!xo_parser_validate("xxx;"));
  CHECK(!xo_parser_validate("xxx a;"));
  CHECK(!xo_parser_validate("xxx a,b;"));

  // invalid arity
  CHECK(!xo_parser_validate("clc a;"));
  CHECK(!xo_parser_validate("clc a,b;"));
  CHECK(!xo_parser_validate("dec;"));
  CHECK(!xo_parser_validate("dec a,b;"));
  CHECK(!xo_parser_validate("adc;"));
  CHECK(!xo_parser_validate("adc a;"));

  return EXIT_SUCCESS;
}
