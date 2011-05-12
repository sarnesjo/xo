#include <stdio.h>
#include "machine_state.h"

void xo_machine_state_print(const xo_machine_state *st, const char *suffix)
{
  for(int i = 0; i < XO_MACHINE_STATE_NUM_REGS; ++i)
    printf("0x%08x ", st->regs[i]);
  printf("%d %d %d %d %d%s", st->cf, st->of, st->pf, st->sf, st->zf, suffix);
}
