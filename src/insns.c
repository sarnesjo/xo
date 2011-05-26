#include <stdlib.h>
#include "flag_set.h"
#include "insns.h"

// TODO: decouple insn metadata and C impl?

#define DST (1 << 0)
#define SRC (1 << 1)

#define CF XO_FLAG_SET_CF
#define OF XO_FLAG_SET_OF
#define PF XO_FLAG_SET_PF
#define SF XO_FLAG_SET_SF
#define ZF XO_FLAG_SET_ZF

// The code to compute the parity of the LSB was taken from [Sean Eron Anderson's Bit Twiddling Hacks][1]
// and modified slightly to yield a 1 for an *even* number of bits.
// [1]: http://graphics.stanford.edu/~seander/bithacks.html#ParityParallel

#define INSN(NAME, ARITY, IREGS, OREGS, IFLAGS, OFLAGS, DFLAGS, CODE) \
  void insn_##NAME(xo_machine_state *st, size_t r0, size_t r1) \
  { \
    uint32_t tmp = 0; \
    uint32_t dst = st->regs[r0]; \
    uint32_t src = st->regs[r1]; \
    uint32_t cf = !!(st->flags & CF); \
    uint32_t of = !!(st->flags & OF); \
    uint32_t pf = !!(st->flags & PF); \
    uint32_t sf = !!(st->flags & SF); \
    uint32_t zf = !!(st->flags & ZF); \
    CODE; \
    if((OFLAGS) & CF) \
    { \
      st->flags = ((st->flags & ~CF) | cf); \
    } \
    if((OFLAGS) & OF) \
    { \
      st->flags = ((st->flags & ~OF) | (of << 1)); \
    } \
    if((OFLAGS) & PF) \
    { \
      pf = (0x9669 >> ((dst ^ (dst >> 4)) & 0xf)) & 1; \
      st->flags = ((st->flags & ~PF) | (pf << 2)); \
    } \
    if((OFLAGS) & SF) \
    { \
      sf = dst >> 31; \
      st->flags = ((st->flags & ~SF) | (sf << 3)); \
    } \
    if((OFLAGS) & ZF) \
    { \
      zf = !dst; \
      st->flags = ((st->flags & ~ZF) | (zf << 4)); \
    } \
    if(OREGS) \
    { \
      st->regs[r0] = dst; \
    } \
  }

#include "insns.def"

#undef INSN

#define INSN(NAME, ARITY, IREGS, OREGS, IFLAGS, OFLAGS, DFLAGS, CODE) \
  {#NAME, ARITY, IREGS, OREGS, IFLAGS, OFLAGS, DFLAGS, insn_##NAME},

xo_instruction xo_insns[] =
{
#include "insns.def"
};

#undef INSN

#undef DST
#undef SRC

#undef CF
#undef OF
#undef PF
#undef SF
#undef ZF
