#include "check.h"
#include "../src/parser.h"

int main()
{
  // basic invalid input
  CHECK(!xo_parser_validate(NULL));

  // basic valid input
  CHECK(xo_parser_validate(""));
  CHECK(xo_parser_validate("add a,b;"));

  // require comma
  CHECK(!xo_parser_validate("add a b;"));

  // require semicolon
  CHECK(!xo_parser_validate("add a,b"));

  // require args
  CHECK(!xo_parser_validate("add a;"));
  CHECK(!xo_parser_validate("add;"));

  // ignore whitespace
  CHECK(xo_parser_validate(" add a , b ; "));
  CHECK(xo_parser_validate(" add a , b ;"));
  CHECK(xo_parser_validate(" add a , b; "));
  CHECK(xo_parser_validate(" add a , b;"));
  CHECK(xo_parser_validate(" add a ,b ; "));
  CHECK(xo_parser_validate(" add a ,b ;"));
  CHECK(xo_parser_validate(" add a ,b; "));
  CHECK(xo_parser_validate(" add a ,b;"));
  CHECK(xo_parser_validate(" add a, b ; "));
  CHECK(xo_parser_validate(" add a, b ;"));
  CHECK(xo_parser_validate(" add a, b; "));
  CHECK(xo_parser_validate(" add a, b;"));
  CHECK(xo_parser_validate(" add a,b ; "));
  CHECK(xo_parser_validate(" add a,b ;"));
  CHECK(xo_parser_validate(" add a,b; "));
  CHECK(xo_parser_validate(" add a,b;"));
  CHECK(xo_parser_validate("add a , b ; "));
  CHECK(xo_parser_validate("add a , b ;"));
  CHECK(xo_parser_validate("add a , b; "));
  CHECK(xo_parser_validate("add a , b;"));
  CHECK(xo_parser_validate("add a ,b ; "));
  CHECK(xo_parser_validate("add a ,b ;"));
  CHECK(xo_parser_validate("add a ,b; "));
  CHECK(xo_parser_validate("add a ,b;"));
  CHECK(xo_parser_validate("add a, b ; "));
  CHECK(xo_parser_validate("add a, b ;"));
  CHECK(xo_parser_validate("add a, b; "));
  CHECK(xo_parser_validate("add a, b;"));
  CHECK(xo_parser_validate("add a,b ; "));
  CHECK(xo_parser_validate("add a,b ;"));
  CHECK(xo_parser_validate("add a,b; "));
  CHECK(xo_parser_validate("add a,b;"));

  // multiple insns
  CHECK(xo_parser_validate("add a,b; add a,b;"));

  // ignore whitespace
  CHECK(xo_parser_validate(" add a,b; add a,b; "));
  CHECK(xo_parser_validate(" add a,b; add a,b;"));
  CHECK(xo_parser_validate(" add a,b;add a,b; "));
  CHECK(xo_parser_validate(" add a,b;add a,b;"));
  CHECK(xo_parser_validate("add a,b; add a,b; "));
  CHECK(xo_parser_validate("add a,b; add a,b;"));
  CHECK(xo_parser_validate("add a,b;add a,b; "));
  CHECK(xo_parser_validate("add a,b;add a,b;"));

  // require semicolons
  CHECK(!xo_parser_validate("add a,b add a,b"));
  CHECK(!xo_parser_validate("add a,b add a,b;"));
  CHECK(!xo_parser_validate("add a,b; add a,b"));

  // catch trailing junk
  CHECK(!xo_parser_validate("add a,b; a"));
  CHECK(!xo_parser_validate("add a,b; ;"));
  CHECK(!xo_parser_validate("add a,b; ?"));

  return EXIT_SUCCESS;
}
