#include <string.h>
#include "generator.h"
#include "insns.h"
#include "invocation.h"
#include "program.h"
#include "register_set.h"

// TODO: model known flag values, rather than just defined/undefined? ex: if cf is known to be 0, such as after and/or/xor, don't generate adc/sbb

// TODO: generate_ has become far too messy... a rewrite is in order
// TODO: generate_ misbehaves when output_regs == 0; how to handle?
void generate_(xo_program *prog, size_t inv,
    xo_register_set input_regs, xo_register_set output_regs, xo_register_set live_regs, xo_flag_set live_flags,
    xo_generator_callback callback, void *userdata)
{
  if(inv == prog->num_invocations-1)
  {
    for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    {
      xo_instruction *insn = &xo_insns[i];

      // if insn doesn't write to output reg, skip
      if(!insn->output_regs)
        continue;

      // if insn reads flags which are undefined, skip
      if((live_flags & insn->input_flags) != insn->input_flags)
        continue;

      xo_flag_set new_live_flags = ((~insn->output_flags & live_flags) | (insn->output_flags & insn->live_output_flags));

      size_t r0 = xo_register_set_first_live_index(output_regs);

      switch(insn->arity)
      {
        case 1:
          if(live_regs & xo_register_set_from_index(r0))
          {
            xo_invocation_init(&prog->invocations[inv], insn, r0, XO_REGISTER_NONE);
            callback(prog, input_regs, output_regs, userdata);
          }
          break;
        case 2:
          if(strcmp(insn->name, "mov") == 0)
          {
            for(size_t r1 = 0; r1 < XO_NUM_REGISTERS; ++r1)
            {
              if(r0 == r1)
                continue;

              if(live_regs & xo_register_set_from_index(r1))
              {
                xo_invocation_init(&prog->invocations[inv], insn, r0, r1);
                callback(prog, input_regs, output_regs, userdata);
              }
            }
          }
          else
          {
            // sub/sbb/xor can be invoked with a non-live register if r0 == r1
            // TODO: we should not call generate sub/sbb/xor with r0 == r1 normally (see comment about cmov below)
            if((strcmp(insn->name, "sub") == 0) || (strcmp(insn->name, "sbb") == 0) || (strcmp(insn->name, "xor") == 0))
            {
              xo_invocation_init(&prog->invocations[inv], insn, r0, r0);
              callback(prog, input_regs, output_regs, userdata);
            }

            if(live_regs & xo_register_set_from_index(r0))
            {
              for(size_t r1 = 0; r1 < XO_NUM_REGISTERS; ++r1)
              {
                // cmov should never be invoked with r0 == r1
                if((r0 == r1) && (strstr(insn->name, "cmov") == insn->name))
                  continue;

                if(live_regs & xo_register_set_from_index(r1))
                {
                  xo_invocation_init(&prog->invocations[inv], insn, r0, r1);
                  callback(prog, input_regs, output_regs, userdata);
                }
              }
            }
          }
          break;
      }
    }
  }
  else
  {
    for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    {
      xo_instruction *insn = &xo_insns[i];

      // if insn reads flags which are undefined, skip
      if((live_flags & insn->input_flags) != insn->input_flags)
        continue;

      xo_flag_set new_live_flags = ((~insn->output_flags & live_flags) | (insn->output_flags & insn->live_output_flags));

      switch(insn->arity)
      {
        case 0:
          xo_invocation_init(&prog->invocations[inv], insn, XO_REGISTER_NONE, XO_REGISTER_NONE);
          generate_(prog, inv+1, input_regs, output_regs, live_regs, new_live_flags, callback, userdata);
          break;
        case 1:
          for(size_t r0 = 0; r0 < XO_NUM_REGISTERS; ++r0)
          {
            if(live_regs & xo_register_set_from_index(r0))
            {
              xo_invocation_init(&prog->invocations[inv], insn, r0, XO_REGISTER_NONE);
              generate_(prog, inv+1, input_regs, output_regs, live_regs | xo_register_set_from_index(r0), new_live_flags, callback, userdata);
            }
          }
          break;
        case 2:
          if(strcmp(insn->name, "mov") == 0)
          {
            // mov should always write to a dead register...
            {
              size_t r0 = xo_register_set_first_dead_index(live_regs); // TODO: what if all regs are live?

              for(size_t r1 = 0; r1 < XO_NUM_REGISTERS; ++r1)
              {
                if(r0 == r1)
                  continue;

                if(live_regs & xo_register_set_from_index(r1))
                {
                  xo_invocation_init(&prog->invocations[inv], insn, r0, r1);
                  generate_(prog, inv+1, input_regs, output_regs, live_regs | xo_register_set_from_index(r0), new_live_flags, callback, userdata);
                }
              }
            }

            // ... or the output register
            {
              size_t r0 = xo_register_set_first_live_index(output_regs);

              for(size_t r1 = 0; r1 < XO_NUM_REGISTERS; ++r1)
              {
                if(r0 == r1)
                  continue;

                if(live_regs & xo_register_set_from_index(r1))
                {
                  xo_invocation_init(&prog->invocations[inv], insn, r0, r1);
                  generate_(prog, inv+1, input_regs, output_regs, live_regs | xo_register_set_from_index(r0), new_live_flags, callback, userdata);
                }
              }
            }
          }
          else
          {
            // sub/sbb/xor can be invoked with a non-live register if r0 == r1
            // TODO: we should not call generate sub/sbb/xor with r0 == r1 normally (see comment about cmov below)
            if((strcmp(insn->name, "sub") == 0) || (strcmp(insn->name, "sbb") == 0) || (strcmp(insn->name, "xor") == 0))
            {
              size_t r0 = xo_register_set_first_dead_index(live_regs); // TODO: what if all regs are live?

              xo_invocation_init(&prog->invocations[inv], insn, r0, r0);
              generate_(prog, inv+1, input_regs, output_regs, live_regs | xo_register_set_from_index(r0), new_live_flags, callback, userdata);
            }

            for(size_t r0 = 0; r0 < XO_NUM_REGISTERS; ++r0)
            {
              if(live_regs & xo_register_set_from_index(r0))
              {
                for(size_t r1 = 0; r1 < XO_NUM_REGISTERS; ++r1)
                {
                  // cmov should never be invoked with r0 == r1
                  if((r0 == r1) && (strstr(insn->name, "cmov") == insn->name))
                    continue;

                  if(live_regs & xo_register_set_from_index(r1))
                  {
                    xo_invocation_init(&prog->invocations[inv], insn, r0, r1);
                    generate_(prog, inv+1, input_regs, output_regs, live_regs | xo_register_set_from_index(r0), new_live_flags, callback, userdata);
                  }
                }
              }
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
