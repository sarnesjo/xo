#include "check.h"
#include "../src/parser.h"

int main()
{
  // basic invalid input
  CHECK(!xo_parser_validate(NULL));

  // basic valid input
  CHECK(xo_parser_validate(""));
  CHECK(xo_parser_validate("add r0,r1;"));

  // require comma
  CHECK(!xo_parser_validate("add r0 r1;"));

  // require semicolon
  CHECK(!xo_parser_validate("add r0,r1"));

  // require args to be named r[0-7]
  CHECK(xo_parser_validate("add r0,r1;"));
  CHECK(xo_parser_validate("add r2,r3;"));
  CHECK(xo_parser_validate("add r4,r5;"));
  CHECK(xo_parser_validate("add r6,r7;"));
  CHECK(!xo_parser_validate("add r8,r9;"));
  CHECK(!xo_parser_validate("add r10,r11;"));
  CHECK(!xo_parser_validate("add r,r;"));
  CHECK(!xo_parser_validate("add r 0,r 1;"));
  CHECK(!xo_parser_validate("add a0,a1;"));

  // ignore whitespace
  CHECK(xo_parser_validate(" add r0 , r1 ; "));
  CHECK(xo_parser_validate(" add r0 , r1 ;"));
  CHECK(xo_parser_validate(" add r0 , r1; "));
  CHECK(xo_parser_validate(" add r0 , r1;"));
  CHECK(xo_parser_validate(" add r0 ,r1 ; "));
  CHECK(xo_parser_validate(" add r0 ,r1 ;"));
  CHECK(xo_parser_validate(" add r0 ,r1; "));
  CHECK(xo_parser_validate(" add r0 ,r1;"));
  CHECK(xo_parser_validate(" add r0, r1 ; "));
  CHECK(xo_parser_validate(" add r0, r1 ;"));
  CHECK(xo_parser_validate(" add r0, r1; "));
  CHECK(xo_parser_validate(" add r0, r1;"));
  CHECK(xo_parser_validate(" add r0,r1 ; "));
  CHECK(xo_parser_validate(" add r0,r1 ;"));
  CHECK(xo_parser_validate(" add r0,r1; "));
  CHECK(xo_parser_validate(" add r0,r1;"));
  CHECK(xo_parser_validate("add r0 , r1 ; "));
  CHECK(xo_parser_validate("add r0 , r1 ;"));
  CHECK(xo_parser_validate("add r0 , r1; "));
  CHECK(xo_parser_validate("add r0 , r1;"));
  CHECK(xo_parser_validate("add r0 ,r1 ; "));
  CHECK(xo_parser_validate("add r0 ,r1 ;"));
  CHECK(xo_parser_validate("add r0 ,r1; "));
  CHECK(xo_parser_validate("add r0 ,r1;"));
  CHECK(xo_parser_validate("add r0, r1 ; "));
  CHECK(xo_parser_validate("add r0, r1 ;"));
  CHECK(xo_parser_validate("add r0, r1; "));
  CHECK(xo_parser_validate("add r0, r1;"));
  CHECK(xo_parser_validate("add r0,r1 ; "));
  CHECK(xo_parser_validate("add r0,r1 ;"));
  CHECK(xo_parser_validate("add r0,r1; "));
  CHECK(xo_parser_validate("add r0,r1;"));

  // multiple insns
  CHECK(xo_parser_validate("add r0,r1; add r0,r1;"));

  // ignore whitespace
  CHECK(xo_parser_validate(" add r0,r1; add r0,r1; "));
  CHECK(xo_parser_validate(" add r0,r1; add r0,r1;"));
  CHECK(xo_parser_validate(" add r0,r1;add r0,r1; "));
  CHECK(xo_parser_validate(" add r0,r1;add r0,r1;"));
  CHECK(xo_parser_validate("add r0,r1; add r0,r1; "));
  CHECK(xo_parser_validate("add r0,r1; add r0,r1;"));
  CHECK(xo_parser_validate("add r0,r1;add r0,r1; "));
  CHECK(xo_parser_validate("add r0,r1;add r0,r1;"));

  // require semicolons
  CHECK(!xo_parser_validate("add r0,r1 add r0,r1"));
  CHECK(!xo_parser_validate("add r0,r1 add r0,r1;"));
  CHECK(!xo_parser_validate("add r0,r1; add r0,r1"));

  // catch trailing junk
  CHECK(!xo_parser_validate("add r0,r1; r0"));
  CHECK(!xo_parser_validate("add r0,r1; r"));
  CHECK(!xo_parser_validate("add r0,r1; a"));
  CHECK(!xo_parser_validate("add r0,r1; ;"));
  CHECK(!xo_parser_validate("add r0,r1; ?"));
  CHECK(!xo_parser_validate("add r0,r1;r0"));
  CHECK(!xo_parser_validate("add r0,r1;r"));
  CHECK(!xo_parser_validate("add r0,r1;a"));
  CHECK(!xo_parser_validate("add r0,r1;;"));
  CHECK(!xo_parser_validate("add r0,r1;?"));

  return EXIT_SUCCESS;
}
