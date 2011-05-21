#include "generator.h"
#include "insns.h"
#include "invocation.h"
#include "program.h"

void generate_(xo_program *prog, size_t inv, xo_generator_callback callback, void *userdata)
{
  if(inv == prog->num_invocations)
  {
    callback(prog, userdata);
  }
  else
  {
    for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    {
      xo_instruction *insn = &xo_insns[i];

      switch(insn->arity)
      {
        case 0:
          xo_invocation_init(&prog->invocations[inv], insn, XO_REGISTER_NONE, XO_REGISTER_NONE);
          generate_(prog, inv+1, callback, userdata);
          break;
        case 1:
          for(size_t r0 = 0; r0 < XO_NUM_REGISTERS; ++r0)
          {
            xo_invocation_init(&prog->invocations[inv], insn, r0, XO_REGISTER_NONE);
            generate_(prog, inv+1, callback, userdata);
          }
          break;
        case 2:
          for(size_t r0 = 0; r0 < XO_NUM_REGISTERS; ++r0)
          {
            for(size_t r1 = 0; r1 < XO_NUM_REGISTERS; ++r1)
            {
              xo_invocation_init(&prog->invocations[inv], insn, r0, r1);
              generate_(prog, inv+1, callback, userdata);
            }
          }
          break;
      }
    }
  }
}

void xo_generator_generate_programs(size_t num_invocations, xo_generator_callback callback, void *userdata)
{
  xo_program *prog = xo_program_create(num_invocations);
  generate_(prog, 0, callback, userdata);
  xo_program_destroy(prog);
}
