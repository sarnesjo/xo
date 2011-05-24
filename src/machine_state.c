#include "machine_state.h"

void xo_machine_state_print(FILE *file, const xo_machine_state *st, const char *suffix)
{
  for(int i = 0; i < XO_NUM_REGISTERS; ++i)
    fprintf(file, "0x%08x ", st->regs[i]);
  fprintf(file, "%d %d %d %d %d%s", st->cf, st->of, st->pf, st->sf, st->zf, suffix);
}
