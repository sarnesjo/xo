#include "check.h"
#include "../src/equivalence_checker_bdd.h"
#include "../src/equivalence_checker_c.h"
#include "../src/program.h"
#include "../src/test_states.h"

static bool equivalent(const char *prog_str_1, const char *prog_str_2)
{
  xo_program *prog1 = xo_program_create_from_str(prog_str_1);
  xo_program *prog2 = xo_program_create_from_str(prog_str_2);

#if XO_TEST_C

  bool equiv = xo_equivalence_checker_c_programs_equivalent_on_states(prog1, prog2, XO_NUM_TEST_STATES, xo_test_states);

#elif XO_TEST_BDD

  bool equiv = xo_equivalence_checker_bdd_programs_equivalent(prog1, prog2);

#else
#error
#endif

  xo_program_destroy(prog1);
  xo_program_destroy(prog2);

  return equiv;
}

int main()
{
  // valid programs are equivalent to themselves
  CHECK(equivalent("add r0,r1;", "add r0,r1;"));
  CHECK(equivalent("sub r0,r1;", "sub r0,r1;"));
  CHECK(equivalent("inc r0;",    "inc r0;"));
  CHECK(equivalent("dec r0;",    "dec r0;"));
  CHECK(equivalent("and r0,r1;", "and r0,r1;"));
  CHECK(equivalent("or  r0,r1;", "or  r0,r1;"));
  CHECK(equivalent("xor r0,r1;", "xor r0,r1;"));
  CHECK(equivalent("not r0;",    "not r0;"));
  CHECK(equivalent("mov r0,r1;", "mov r0,r1;"));

  // programs with different output registers are not equivalent
  CHECK(!equivalent("add r0,r1;", "add r1,r0;"));
  CHECK(!equivalent("sub r0,r1;", "sub r1,r0;"));
  CHECK(!equivalent("inc r0;",    "inc r1;"));
  CHECK(!equivalent("dec r0;",    "dec r1;"));
  CHECK(!equivalent("and r0,r1;", "and r1,r0;"));
  CHECK(!equivalent("or  r0,r1;", "or  r1,r0;"));
  CHECK(!equivalent("xor r0,r1;", "xor r1,r0;"));
  CHECK(!equivalent("not r0;",    "not r1;"));
  CHECK(!equivalent("mov r0,r1;", "mov r1,r0;"));

  // sanity check: obvious non-equivalence
  CHECK(!equivalent("add r0,r1;", "sub r0,r1;"));
  CHECK(!equivalent("and r0,r1;", "or r0,r1;"));
  CHECK(!equivalent("inc r0;", "dec r0;"));

  // ((r0 + r1) + r2) + r3 is equivalent to (r0 + r1) + (r2 + 3)
  CHECK(equivalent("add r0,r1; add r0,r2; add r0,r3;", "add r0,r1; add r2,r3; add r0,r2;"));

  // (r0 - r1) - r2 is equivalent to r0 - (r1 + r2)
  CHECK(equivalent("sub r0,r1; sub r0,r2;", "add r1,r2; sub r0,r1;"));

  // ~(r0 & r1) is equivalent to ~r0 | ~r1 (De Morgan)
  CHECK(equivalent("and r0,r1; not r0;", "not r0; not r1; or r0,r1;"));

  // two ways to clear r0
  CHECK(equivalent("sub r0,r0;", "xor r0,r0;"));

  // a few cases where cmov can be replaced by mov
  CHECK(equivalent("stc; cmovc r0,r1;",        "mov r0,r1;"));
  CHECK(equivalent("clc; cmovnc r0,r1;",       "mov r0,r1;"));
  CHECK(equivalent("cmp r0,r1; cmovnz r0,r1;", "mov r0,r1;"));

  // add != adc, sub != sbb
  CHECK(!equivalent("add r0,r1; add r0,r2;", "add r0,r1; adc r0,r2;"));
  CHECK(!equivalent("sub r0,r1; sub r0,r2;", "sub r0,r1; sbb r0,r2;"));

  // l != le, g != ge, a != ae (nc), b (c) != be
  // a probabilistic equivalence checker that uses only random input states will probably fail here
  CHECK(!equivalent("cmp r0,r1; cmovl r2,r3;", "cmp r0,r1; cmovle r2,r3;"));
  CHECK(!equivalent("cmp r0,r1; cmovg r2,r3;", "cmp r0,r1; cmovge r2,r3;"));
  CHECK(!equivalent("cmp r0,r1; cmova r2,r3;", "cmp r0,r1; cmovnc r2,r3;"));
  CHECK(!equivalent("cmp r0,r1; cmovc r2,r3;", "cmp r0,r1; cmovbe r2,r3;"));

  // the venerable sign program, r3 = sign(r3)
  CHECK(equivalent("xor r0,r0; xor r1,r1; xor r2,r2; inc r1; dec r2; cmp r3,r0; cmovg r3,r1; cmovl r3,r2;",
        "add r3,r3; sbb r0,r0; sub r0,r3; adc r3,r0;"));

  // an incorrect variant of the sign program, which misbehaves when r3 == 0
  CHECK(!equivalent("xor r0,r0; xor r1,r1; xor r2,r2; inc r1; dec r2; cmp r3,r0; cmovg r3,r1; cmovl r3,r2;",
        "add r3,r3; sbb r3,r3; add r3,r3; inc r3;"));

  return EXIT_SUCCESS;
}
