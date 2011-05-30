#include "check.h"
#include "../src/bdd_wrapper.hpp"
#include "../src/equivalence_checker_c.h"
#include "../src/flag_set.h"
#include "../src/machine_state.h"
#include "../src/program.h"

#define CF XO_FLAG_SET_CF
#define OF XO_FLAG_SET_OF
#define PF XO_FLAG_SET_PF
#define SF XO_FLAG_SET_SF
#define ZF XO_FLAG_SET_ZF

static bool expected_output(const char *input_prog_str,
    uint32_t input_r0, uint32_t input_r1, xo_flag_set input_flags,
    uint32_t expected_output_r0, xo_flag_set expected_output_flags)
{
  xo_program *prog = xo_program_create_from_str(input_prog_str);

  xo_machine_state st;
  xo_machine_state_init(&st, 0xdeadbeef, input_flags);
  st.regs[0] = input_r0;
  st.regs[1] = input_r1;

#if XO_TEST_C

  xo_equivalence_checker_c_run_program_on_state(prog, &st);

#elif XO_TEST_BDD

  xo_bdd_evaluate_program_on_state(prog, &st);

#else
#error
#endif

  if(st.regs[0] != expected_output_r0 || st.flags != expected_output_flags)
  {
    fprintf(stderr, "0x%08x != 0x%08x\n", st.regs[0], expected_output_r0);
    xo_flag_set_print(stderr, st.flags, " != ");
    xo_flag_set_print(stderr, expected_output_flags, "\n");
    return false;
  }

  return true;
}

int main()
{
  // arithmetic

  CHECK(expected_output("add r0,r1;",    0x31415926, 0x27182818, 0,  0x5859813e, 0));
  CHECK(expected_output("add r0,r1;",    0x31415926, 0x27182818, CF, 0x5859813e, 0));
  CHECK(expected_output("add r0,r1;",    0xffffffff, 0x00000001, 0,  0x00000000, CF|PF|ZF));
  CHECK(expected_output("add r0,r1;",    0x7fffffff, 0x00000001, 0,  0x80000000, OF|PF|SF));

  CHECK(expected_output("adc r0,r1;",    0x31415926, 0x27182818, 0,  0x5859813e, 0));
  CHECK(expected_output("adc r0,r1;",    0x31415926, 0x27182818, CF, 0x5859813f, PF));
  CHECK(expected_output("adc r0,r1;",    0x7fffffff, 0x00000000, 0,  0x7fffffff, PF));
  CHECK(expected_output("adc r0,r1;",    0x7fffffff, 0x00000000, CF, 0x80000000, OF|PF|SF));
  CHECK(expected_output("adc r0,r1;",    0xffffffff, 0x00000000, 0,  0xffffffff, PF|SF));
  CHECK(expected_output("adc r0,r1;",    0xffffffff, 0x00000000, CF, 0x00000000, CF|PF|ZF));

  CHECK(expected_output("sub r0,r1;",    0x31415926, 0x27182818, 0,  0x0a29310e, 0));
  CHECK(expected_output("sub r0,r1;",    0x31415926, 0x27182818, CF, 0x0a29310e, 0));
  CHECK(expected_output("sub r0,r1;",    0x00000000, 0x00000001, 0,  0xffffffff, CF|PF|SF));
  CHECK(expected_output("sub r0,r1;",    0x80000000, 0x00000001, 0,  0x7fffffff, OF|PF));

  CHECK(expected_output("sbb r0,r1;",    0x31415926, 0x27182818, 0,  0x0a29310e, 0));
  CHECK(expected_output("sbb r0,r1;",    0x31415926, 0x27182818, CF, 0x0a29310d, 0));
  CHECK(expected_output("sbb r0,r1;",    0x00000000, 0x00000000, 0,  0x00000000, PF|ZF));
  CHECK(expected_output("sbb r0,r1;",    0x00000000, 0x00000000, CF, 0xffffffff, CF|PF|SF));
  CHECK(expected_output("sbb r0,r1;",    0x80000000, 0x00000000, 0,  0x80000000, PF|SF));
  CHECK(expected_output("sbb r0,r1;",    0x80000000, 0x00000000, CF, 0x7fffffff, OF|PF));

  CHECK(expected_output("cmp r0,r1;",    0x31415926, 0x27182818, 0,  0x31415926, 0));
  CHECK(expected_output("cmp r0,r1;",    0x31415926, 0x27182818, CF, 0x31415926, 0));
  CHECK(expected_output("cmp r0,r1;",    0x00000000, 0x00000001, 0,  0x00000000, CF|PF|SF));
  CHECK(expected_output("cmp r0,r1;",    0x80000000, 0x00000001, 0,  0x80000000, OF|PF));

  CHECK(expected_output("inc r0;",       0x31415926, 0x00000000, 0,  0x31415927, PF));
  CHECK(expected_output("inc r0;",       0x31415926, 0x00000000, CF, 0x31415927, CF|PF));
  CHECK(expected_output("inc r0;",       0xffffffff, 0x00000000, 0,  0x00000000, PF|ZF));
  CHECK(expected_output("inc r0;",       0xffffffff, 0x00000000, CF, 0x00000000, CF|PF|ZF));
  CHECK(expected_output("inc r0;",       0x7fffffff, 0x00000000, 0,  0x80000000, OF|PF|SF));
  CHECK(expected_output("inc r0;",       0x7fffffff, 0x00000000, CF, 0x80000000, CF|OF|PF|SF));

  CHECK(expected_output("dec r0;",       0x31415926, 0x00000000, 0,  0x31415925, 0));
  CHECK(expected_output("dec r0;",       0x31415926, 0x00000000, CF, 0x31415925, CF));
  CHECK(expected_output("dec r0;",       0x00000000, 0x00000000, 0,  0xffffffff, PF|SF));
  CHECK(expected_output("dec r0;",       0x00000000, 0x00000000, CF, 0xffffffff, CF|PF|SF));
  CHECK(expected_output("dec r0;",       0x80000000, 0x00000000, 0,  0x7fffffff, OF|PF));
  CHECK(expected_output("dec r0;",       0x80000000, 0x00000000, CF, 0x7fffffff, CF|OF|PF));

  // logic

  CHECK(expected_output("and r0,r1;",    0x00000000, 0x00000000, 0,              0x00000000, PF|ZF));
  CHECK(expected_output("and r0,r1;",    0x00000000, 0x00000000, CF|OF|PF|SF|ZF, 0x00000000, PF|ZF));
  CHECK(expected_output("and r0,r1;",    0x00ffffff, 0xff00ffff, 0,              0x0000ffff, PF));
  CHECK(expected_output("and r0,r1;",    0x00ffffff, 0xff00ffff, CF|OF|PF|SF|ZF, 0x0000ffff, PF));

  CHECK(expected_output("or  r0,r1;",    0x00000000, 0x00000000, 0,              0x00000000, PF|ZF));
  CHECK(expected_output("or  r0,r1;",    0x00000000, 0x00000000, CF|OF|PF|SF|ZF, 0x00000000, PF|ZF));
  CHECK(expected_output("or  r0,r1;",    0x00ffffff, 0xff00ffff, 0,              0xffffffff, PF|SF));
  CHECK(expected_output("or  r0,r1;",    0x00ffffff, 0xff00ffff, CF|OF|PF|SF|ZF, 0xffffffff, PF|SF));

  CHECK(expected_output("xor r0,r1;",    0x00000000, 0x00000000, 0,              0x00000000, PF|ZF));
  CHECK(expected_output("xor r0,r1;",    0x00000000, 0x00000000, CF|OF|PF|SF|ZF, 0x00000000, PF|ZF));
  CHECK(expected_output("xor r0,r1;",    0x00ffffff, 0xff00ffff, 0,              0xffff0000, PF|SF));
  CHECK(expected_output("xor r0,r1;",    0x00ffffff, 0xff00ffff, CF|OF|PF|SF|ZF, 0xffff0000, PF|SF));

  CHECK(expected_output("not r0;",       0x00000000, 0x00000000, 0,              0xffffffff, 0));
  CHECK(expected_output("not r0;",       0x00000000, 0x00000000, CF|OF|PF|SF|ZF, 0xffffffff, CF|OF|PF|SF|ZF));
  CHECK(expected_output("not r0;",       0xffffffff, 0x00000000, 0,              0x00000000, 0));
  CHECK(expected_output("not r0;",       0xffffffff, 0x00000000, CF|OF|PF|SF|ZF, 0x00000000, CF|OF|PF|SF|ZF));

  // flag control

  CHECK(expected_output("stc;",          0x00000000, 0x00000000, 0,              0x00000000, CF));
  CHECK(expected_output("stc;",          0x00000000, 0x00000000, CF|OF|PF|SF|ZF, 0x00000000, CF|OF|PF|SF|ZF));

  CHECK(expected_output("clc;",          0x00000000, 0x00000000, 0,              0x00000000, 0));
  CHECK(expected_output("clc;",          0x00000000, 0x00000000, CF|OF|PF|SF|ZF, 0x00000000, OF|PF|SF|ZF));

  CHECK(expected_output("cmc;",          0x00000000, 0x00000000, 0,              0x00000000, CF));
  CHECK(expected_output("cmc;",          0x00000000, 0x00000000, CF|OF|PF|SF|ZF, 0x00000000, OF|PF|SF|ZF));

  // move

  CHECK(expected_output("mov r0,r1;",    0x31415926, 0x27182818, 0,              0x27182818, 0));
  CHECK(expected_output("mov r0,r1;",    0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x27182818, CF|OF|PF|SF|ZF));

  // conditional move

  CHECK(expected_output("cmovc  r0,r1;", 0x31415926, 0x27182818, 0,              0x31415926, 0));
  CHECK(expected_output("cmovc  r0,r1;", 0x31415926, 0x27182818, CF,             0x27182818, CF));
  CHECK(expected_output("cmovc  r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x27182818, CF|OF|PF|SF|ZF));

  CHECK(expected_output("cmovo  r0,r1;", 0x31415926, 0x27182818, 0,              0x31415926, 0));
  CHECK(expected_output("cmovo  r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x27182818, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovo  r0,r1;", 0x31415926, 0x27182818, OF,             0x27182818, OF));

  CHECK(expected_output("cmovp  r0,r1;", 0x31415926, 0x27182818, 0,              0x31415926, 0));
  CHECK(expected_output("cmovp  r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x27182818, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovp  r0,r1;", 0x31415926, 0x27182818, PF,             0x27182818, PF));

  CHECK(expected_output("cmovs  r0,r1;", 0x31415926, 0x27182818, 0,              0x31415926, 0));
  CHECK(expected_output("cmovs  r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x27182818, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovs  r0,r1;", 0x31415926, 0x27182818, SF,             0x27182818, SF));

  CHECK(expected_output("cmovz  r0,r1;", 0x31415926, 0x27182818, 0,              0x31415926, 0));
  CHECK(expected_output("cmovz  r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x27182818, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovz  r0,r1;", 0x31415926, 0x27182818, ZF,             0x27182818, ZF));

  CHECK(expected_output("cmovnc r0,r1;", 0x31415926, 0x27182818, 0,              0x27182818, 0));
  CHECK(expected_output("cmovnc r0,r1;", 0x31415926, 0x27182818, CF,             0x31415926, CF));
  CHECK(expected_output("cmovnc r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x31415926, CF|OF|PF|SF|ZF));

  CHECK(expected_output("cmovno r0,r1;", 0x31415926, 0x27182818, 0,              0x27182818, 0));
  CHECK(expected_output("cmovno r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x31415926, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovno r0,r1;", 0x31415926, 0x27182818, OF,             0x31415926, OF));

  CHECK(expected_output("cmovnp r0,r1;", 0x31415926, 0x27182818, 0,              0x27182818, 0));
  CHECK(expected_output("cmovnp r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x31415926, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovnp r0,r1;", 0x31415926, 0x27182818, PF,             0x31415926, PF));

  CHECK(expected_output("cmovns r0,r1;", 0x31415926, 0x27182818, 0,              0x27182818, 0));
  CHECK(expected_output("cmovns r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x31415926, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovns r0,r1;", 0x31415926, 0x27182818, SF,             0x31415926, SF));

  CHECK(expected_output("cmovnz r0,r1;", 0x31415926, 0x27182818, 0,              0x27182818, 0));
  CHECK(expected_output("cmovnz r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x31415926, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovnz r0,r1;", 0x31415926, 0x27182818, ZF,             0x31415926, ZF));

  CHECK(expected_output("cmova  r0,r1;", 0x31415926, 0x27182818, 0,              0x27182818, 0));
  CHECK(expected_output("cmova  r0,r1;", 0x31415926, 0x27182818, CF,             0x31415926, CF));
  CHECK(expected_output("cmova  r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x31415926, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmova  r0,r1;", 0x31415926, 0x27182818, CF|ZF,          0x31415926, CF|ZF));
  CHECK(expected_output("cmova  r0,r1;", 0x31415926, 0x27182818, ZF,             0x31415926, ZF));

  CHECK(expected_output("cmovbe r0,r1;", 0x31415926, 0x27182818, 0,              0x31415926, 0));
  CHECK(expected_output("cmovbe r0,r1;", 0x31415926, 0x27182818, CF,             0x27182818, CF));
  CHECK(expected_output("cmovbe r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x27182818, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovbe r0,r1;", 0x31415926, 0x27182818, CF|ZF,          0x27182818, CF|ZF));
  CHECK(expected_output("cmovbe r0,r1;", 0x31415926, 0x27182818, ZF,             0x27182818, ZF));

  CHECK(expected_output("cmovg  r0,r1;", 0x31415926, 0x27182818, 0,              0x27182818, 0));
  CHECK(expected_output("cmovg  r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x31415926, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovg  r0,r1;", 0x31415926, 0x27182818, OF,             0x31415926, OF));
  CHECK(expected_output("cmovg  r0,r1;", 0x31415926, 0x27182818, OF|SF,          0x27182818, OF|SF));
  CHECK(expected_output("cmovg  r0,r1;", 0x31415926, 0x27182818, OF|SF|ZF,       0x31415926, OF|SF|ZF));
  CHECK(expected_output("cmovg  r0,r1;", 0x31415926, 0x27182818, OF|ZF,          0x31415926, OF|ZF));
  CHECK(expected_output("cmovg  r0,r1;", 0x31415926, 0x27182818, SF,             0x31415926, SF));
  CHECK(expected_output("cmovg  r0,r1;", 0x31415926, 0x27182818, SF|ZF,          0x31415926, SF|ZF));
  CHECK(expected_output("cmovg  r0,r1;", 0x31415926, 0x27182818, ZF,             0x31415926, ZF));

  CHECK(expected_output("cmovge r0,r1;", 0x31415926, 0x27182818, 0,              0x27182818, 0));
  CHECK(expected_output("cmovge r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x27182818, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovge r0,r1;", 0x31415926, 0x27182818, OF,             0x31415926, OF));
  CHECK(expected_output("cmovge r0,r1;", 0x31415926, 0x27182818, OF|SF,          0x27182818, OF|SF));
  CHECK(expected_output("cmovge r0,r1;", 0x31415926, 0x27182818, SF,             0x31415926, SF));

  CHECK(expected_output("cmovl  r0,r1;", 0x31415926, 0x27182818, 0,              0x31415926, 0));
  CHECK(expected_output("cmovl  r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x31415926, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovl  r0,r1;", 0x31415926, 0x27182818, OF,             0x27182818, OF));
  CHECK(expected_output("cmovl  r0,r1;", 0x31415926, 0x27182818, OF|SF,          0x31415926, OF|SF));
  CHECK(expected_output("cmovl  r0,r1;", 0x31415926, 0x27182818, SF,             0x27182818, SF));

  CHECK(expected_output("cmovle r0,r1;", 0x31415926, 0x27182818, 0,              0x31415926, 0));
  CHECK(expected_output("cmovle r0,r1;", 0x31415926, 0x27182818, CF|OF|PF|SF|ZF, 0x27182818, CF|OF|PF|SF|ZF));
  CHECK(expected_output("cmovle r0,r1;", 0x31415926, 0x27182818, OF,             0x27182818, OF));
  CHECK(expected_output("cmovle r0,r1;", 0x31415926, 0x27182818, OF|SF,          0x31415926, OF|SF));
  CHECK(expected_output("cmovle r0,r1;", 0x31415926, 0x27182818, OF|SF|ZF,       0x27182818, OF|SF|ZF));
  CHECK(expected_output("cmovle r0,r1;", 0x31415926, 0x27182818, OF|ZF,          0x27182818, OF|ZF));
  CHECK(expected_output("cmovle r0,r1;", 0x31415926, 0x27182818, SF,             0x27182818, SF));
  CHECK(expected_output("cmovle r0,r1;", 0x31415926, 0x27182818, SF|ZF,          0x27182818, SF|ZF));
  CHECK(expected_output("cmovle r0,r1;", 0x31415926, 0x27182818, ZF,             0x27182818, ZF));

  return EXIT_SUCCESS;
}
