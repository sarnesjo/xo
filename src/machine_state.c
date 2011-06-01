#include "flag_set.h"
#include "machine_state.h"

void xo_machine_state_init(xo_machine_state *st, xo_register reg_value, xo_flag_set set_flags)
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

// flag getters

xo_register xo_machine_state_get_cf(const xo_machine_state *st)
{
  return !!(st->flags & XO_FLAG_SET_CF);
}

xo_register xo_machine_state_get_of(const xo_machine_state *st)
{
  return !!(st->flags & XO_FLAG_SET_OF);
}

xo_register xo_machine_state_get_pf(const xo_machine_state *st)
{
  return !!(st->flags & XO_FLAG_SET_PF);
}

xo_register xo_machine_state_get_sf(const xo_machine_state *st)
{
  return !!(st->flags & XO_FLAG_SET_SF);
}

xo_register xo_machine_state_get_zf(const xo_machine_state *st)
{
  return !!(st->flags & XO_FLAG_SET_ZF);
}

// flag setters

void xo_machine_state_set_cf(xo_machine_state *st, xo_register f)
{
  st->flags = (st->flags & ~XO_FLAG_SET_CF) | (f << XO_FLAG_OFFSET_CF);
}

void xo_machine_state_set_of(xo_machine_state *st, xo_register f)
{
  st->flags = (st->flags & ~XO_FLAG_SET_OF) | (f << XO_FLAG_OFFSET_OF);
}

void xo_machine_state_set_pf(xo_machine_state *st, xo_register f)
{
  st->flags = (st->flags & ~XO_FLAG_SET_PF) | (f << XO_FLAG_OFFSET_PF);
}

void xo_machine_state_set_sf(xo_machine_state *st, xo_register f)
{
  st->flags = (st->flags & ~XO_FLAG_SET_SF) | (f << XO_FLAG_OFFSET_SF);
}

void xo_machine_state_set_zf(xo_machine_state *st, xo_register f)
{
  st->flags = (st->flags & ~XO_FLAG_SET_ZF) | (f << XO_FLAG_OFFSET_ZF);
}
