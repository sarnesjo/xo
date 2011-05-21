#include "generator.h"
#include "insns.h"
#include "invocation.h"
#include "program.h"

// TODO: mov should always write to a non-live register
// TODO: mov/cmov should never use r0 == r1
// TODO: sbb/sub/xor with r0 == r1 don't really read input; how to handle?
// TODO: in last round, insn must write to output reg
// TODO: model known flag values, rather than just defined/undefined? ex: if cf is known to be 0, such as after and/or/xor, don't generate adc/sbb

#define R(n) (1 << (n)) // TODO: this should be moved elsewhere

void generate_(xo_program *prog, size_t inv,
    xo_register_set input_regs, xo_register_set output_regs, xo_register_set live_regs, xo_flag_set live_flags,
    xo_generator_callback callback, void *userdata)
{
  if(inv == prog->num_invocations)
  {
    callback(prog, input_regs, output_regs, userdata);
  }
  else
  {
    for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    {
      xo_instruction *insn = &xo_insns[i];

      if((live_flags & insn->input_flags) != insn->input_flags)
        continue;

      xo_flag_set new_live_flags = ((~insn->output_flags & live_flags) | (insn->output_flags & insn->defined_flags));

      switch(insn->arity)
      {
        case 0:
          xo_invocation_init(&prog->invocations[inv], insn, XO_REGISTER_NONE, XO_REGISTER_NONE);
          generate_(prog, inv+1, input_regs, output_regs, live_regs, new_live_flags, callback, userdata);
          break;
        case 1:
          for(size_t r0 = 0; r0 < XO_NUM_REGISTERS; ++r0)
            if(live_regs & R(r0))
            {
              xo_invocation_init(&prog->invocations[inv], insn, r0, XO_REGISTER_NONE);
              generate_(prog, inv+1, input_regs, output_regs, live_regs | R(r0), new_live_flags, callback, userdata);
            }
          break;
        case 2:
          for(size_t r0 = 0; r0 < XO_NUM_REGISTERS; ++r0)
            if(live_regs & R(r0))
            {
              for(size_t r1 = 0; r1 < XO_NUM_REGISTERS; ++r1)
                if(live_regs & R(r1))
                {
                  xo_invocation_init(&prog->invocations[inv], insn, r0, r1);
                  generate_(prog, inv+1, input_regs, output_regs, live_regs | R(r0), new_live_flags, callback, userdata);
                }
            }
          break;
      }
    }
  }
}

void xo_generator_generate_programs(size_t num_invocations, xo_register_set input_regs, xo_register_set output_regs,
    xo_generator_callback callback, void *userdata)
{
  xo_program *prog = xo_program_create(num_invocations);
  generate_(prog, 0, input_regs, output_regs, input_regs, 0, callback, userdata);
  xo_program_destroy(prog);
}
