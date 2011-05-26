#include "flag_set.h"
#include "machine_state.h"

void xo_machine_state_print(FILE *file, const xo_machine_state *st, const char *suffix)
{
  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    fprintf(file, "0x%08x ", st->regs[i]);
  xo_flag_set_print(file, st->flags, suffix);
}
