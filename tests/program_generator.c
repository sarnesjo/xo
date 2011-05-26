#include "check.h"
#include "../src/generator.h"
#include "../src/program.h"
#include "../src/register_set.h"

#define R0 XO_REGISTER_SET_R0
#define R1 XO_REGISTER_SET_R1
#define R2 XO_REGISTER_SET_R2
#define R3 XO_REGISTER_SET_R3
#define R4 XO_REGISTER_SET_R4
#define R5 XO_REGISTER_SET_R5
#define R6 XO_REGISTER_SET_R6
#define R7 XO_REGISTER_SET_R7

typedef struct
{
  xo_program *prog;
  bool did_generate_prog;
} test_userdata;

static void did_generate_prog(const xo_program *prog, xo_register_set input_regs, xo_register_set output_regs, void *userdata)
{
  test_userdata *tu = userdata;
  if(xo_program_equal(prog, tu->prog))
    tu->did_generate_prog = true;
}

static bool generates_self(const char *prog_str)
{
  test_userdata tu;
  tu.prog = xo_program_create_from_str(prog_str);
  tu.did_generate_prog = false;
  xo_register_set input_regs = 0, output_regs = 0;
  xo_program_analyze(tu.prog, &input_regs, &output_regs);
  xo_generator_generate_programs(tu.prog->num_invocations, input_regs, output_regs, did_generate_prog, &tu);
  xo_program_destroy(tu.prog);
  return tu.did_generate_prog;
}

static bool generates(xo_register_set input_regs, xo_register_set output_regs, const char *expected_prog_str)
{
  test_userdata tu;
  tu.prog = xo_program_create_from_str(expected_prog_str);
  tu.did_generate_prog = false;
  xo_generator_generate_programs(tu.prog->num_invocations, input_regs, output_regs, did_generate_prog, &tu);
  xo_program_destroy(tu.prog);
  return tu.did_generate_prog;
}

int main()
{
  CHECK(generates_self("add r0,r1;"));
  CHECK(generates_self("sub r0,r1;"));
  CHECK(generates_self("inc r0;"));
  CHECK(generates_self("dec r0;"));
  CHECK(generates_self("and r0,r1;"));
  CHECK(generates_self("or  r0,r1;"));
  CHECK(generates_self("xor r0,r1;"));
  CHECK(generates_self("not r0;"));
  CHECK(generates_self("mov r0,r1;"));

  CHECK(generates_self("stc; adc r0,r1;"));
  CHECK(generates_self("stc; sbb r0,r1;"));

  CHECK(generates_self("stc; cmovc  r0,r1;"));
  CHECK(generates_self("clc; cmovnc r0,r1;"));
  CHECK(generates_self("add r0,r1; cmc; adc r0,r1;"));

  CHECK(generates_self("cmp r0,r1; cmovc  r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovo  r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovp  r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovs  r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovz  r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovnc r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovno r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovnp r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovns r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovnz r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmova  r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovbe r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovg  r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovge r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovl  r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovle r0,r1;"));

  CHECK(generates_self("sub r0,r0;"));
  CHECK(generates_self("xor r0,r0;"));
  CHECK(generates_self("stc; sbb r0,r0;"));

  // instructions may only read from live registers

  CHECK(generates(R0|R1, R0, "add r0,r0;"));
  CHECK(generates(R0|R1, R0, "add r0,r1;"));

  CHECK(!generates(R0|R1, R0, "add r0,r2;"));
  CHECK(!generates(R0|R1, R0, "add r0,r3;"));
  CHECK(!generates(R0|R1, R0, "add r0,r4;"));
  CHECK(!generates(R0|R1, R0, "add r0,r5;"));
  CHECK(!generates(R0|R1, R0, "add r0,r6;"));
  CHECK(!generates(R0|R1, R0, "add r0,r7;"));

  CHECK(!generates(R0|R1, R0, "add r0,r2; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r3; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r4; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r5; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r6; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r7; not r0;"));

  // instructions may only read from live flags

  CHECK(!generates(R0|R1, R0, "adc r0,r1;"));
  CHECK(!generates(R0|R1, R0, "sbb r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmc;"));
  CHECK(!generates(R0|R1, R0, "cmovc  r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovo  r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovp  r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovs  r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovz  r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovnc r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovno r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovnp r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovns r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovnz r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmova  r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovbe r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovg  r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovge r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovl  r0,r1;"));
  CHECK(!generates(R0|R1, R0, "cmovle r0,r1;"));

  CHECK(!generates(R0|R1, R0, "adc r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "sbb r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmc; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovc  r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovo  r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovp  r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovs  r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovz  r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovnc r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovno r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovnp r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovns r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovnz r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmova  r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovbe r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovg  r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovge r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovl  r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "cmovle r0,r1; not r0;"));

  // last instruction in program must write to output register

  CHECK(generates(R0|R1, R0, "add r0,r1;"));

  CHECK(!generates(R0|R1, R0, "add r1,r0;"));
  CHECK(!generates(R0|R1, R0, "stc;"));
  CHECK(!generates(R0|R1, R0, "clc;"));
  CHECK(!generates(R0|R1, R0, "cmc;"));
  CHECK(!generates(R0|R1, R0, "cmp r0,r1;"));

  CHECK(!generates(R0|R1, R0, "add r0,r1; add r1,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; stc;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; clc;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmc;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmp r0,r1;"));

  // mov should never write to a live register, except the output register

  CHECK(generates(R0|R1, R0, "mov r0,r1;"));
  CHECK(generates(R0|R1, R0, "mov r0,r1; not r0;"));
  CHECK(!generates(R0|R1, R0, "mov r1,r0; not r0;"));

  // mov/cmov should never be invoked with r0 == r1

  CHECK(!generates(R0|R1, R0, "mov r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovc  r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovo  r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovp  r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovs  r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovz  r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovnc r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovno r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovnp r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovns r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovnz r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmova  r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovbe r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovg  r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovge r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovl  r0,r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovle r0,r0;"));

  CHECK(!generates(R0|R1, R0, "mov r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovc  r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovo  r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovp  r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovs  r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovz  r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovnc r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovno r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovnp r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovns r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovnz r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmova  r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovbe r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovg  r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovge r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovl  r0,r0; not r0;"));
  CHECK(!generates(R0|R1, R0, "add r0,r1; cmovle r0,r0; not r0;"));

  // sub/sbb/xor may be invoked with a non-live register if r0 == r1
  // these tests assume that the non-live register with lowest index will be used

  CHECK(generates(0, R0, "sub r0,r0;"));
  CHECK(generates(0, R0, "xor r0,r0;"));
  CHECK(generates(0, R0, "stc; sbb r0,r0;"));

  CHECK(generates(0, R0, "sub r0,r0; not r0;"));
  CHECK(generates(0, R0, "xor r0,r0; not r0;"));
  CHECK(generates(0, R0, "stc; sbb r0,r0; not r0;"));

  CHECK(generates(R0, R0, "sub r1,r1; sbb r0,r0;"));
  CHECK(generates(R0, R0, "xor r1,r1; sbb r0,r0;"));
  CHECK(generates(R0, R0, "stc; sbb r1,r1; sbb r0,r0;"));

  return EXIT_SUCCESS;
}
