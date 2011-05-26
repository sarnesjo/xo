#include "flag_set.h"
#include "machine_state.h"

void xo_machine_state_init(xo_machine_state *st, uint32_t reg_value, xo_flag_set set_flags)
{
  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    st->regs[i] = reg_value;
  st->flags = set_flags;
}

void xo_machine_state_copy(xo_machine_state *st1, const xo_machine_state *st2)
{
  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    st1->regs[i] = st2->regs[i];
  st1->flags = st2->flags;
}

void xo_machine_state_print(FILE *file, const xo_machine_state *st, const char *suffix)
{
  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    fprintf(file, "0x%08x ", st->regs[i]);
  xo_flag_set_print(file, st->flags, suffix);
}
