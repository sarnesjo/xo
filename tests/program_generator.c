#include "check.h"
#include "../src/generator.h"
#include "../src/program.h"

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

int main()
{
  CHECK(generates_self("add r0,r1;"));
  CHECK(generates_self("sub r0,r1;"));
  CHECK(generates_self("inc r0;"));
  CHECK(generates_self("dec r0;"));
  CHECK(generates_self("and r0,r1;"));
  CHECK(generates_self("or r0,r1;"));
  CHECK(generates_self("xor r0,r1;"));
  CHECK(generates_self("not r0;"));
  CHECK(generates_self("mov r0,r1;"));

  CHECK(generates_self("stc; adc r0,r1;"));
  CHECK(generates_self("stc; sbb r0,r1;"));

  CHECK(generates_self("stc; cmovc r0,r1;"));
  CHECK(generates_self("clc; cmovnc r0,r1;"));
  CHECK(generates_self("add r0,r1; cmc; adc r0,r1;"));

  CHECK(generates_self("cmp r0,r1; cmovc r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovo r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovp r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovs r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovz r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovnc r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovno r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovnp r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovns r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovnz r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmova r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovbe r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovg r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovge r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovl r0,r1;"));
  CHECK(generates_self("cmp r0,r1; cmovle r0,r1;"));

  CHECK(generates_self("sub r0,r0;"));
  CHECK(generates_self("xor r0,r0;"));
  CHECK(generates_self("stc; sbb r0,r0;"));

  return EXIT_SUCCESS;
}
