#include <stdlib.h>
#include "insns.h"

// TODO: model undefined flag values
// TODO: model imm
// TODO: do we need to model signed/unsigned?
// TODO: do we need to model atomicity? see LOCK

// The code to compute the parity of the LSB was taken from [Sean Eron Anderson's Bit Twiddling Hacks][1]
// and modified slightly to yield a 1 for an *even* number of bits.
// [1]: http://graphics.stanford.edu/~seander/bithacks.html#ParityParallel

#define _ 0 // discard, do not update register/flag
#define W 1 // write, update register/flag with the value assigned to it by the code snippet
#define M 2 // (PF, SF, ZF only) modify, update flag automatically, based on the value assigned to dst by the code snippet

#define INSN2(NAME, DST, CF, OF, PF, SF, ZF, CODE)                         \
void insn_##NAME(xo_machine_state *state, size_t r0, size_t r1)            \
{                                                                          \
  uint32_t dst = state->regs[r0];                                          \
  uint32_t src = state->regs[r1];                                          \
  uint32_t cf = state->cf;                                                 \
  uint32_t of = state->of;                                                 \
  uint32_t pf = state->pf;                                                 \
  uint32_t sf = state->sf;                                                 \
  uint32_t zf = state->zf;                                                 \
  uint32_t tmp;                                                            \
                                                                           \
  CODE;                                                                    \
                                                                           \
  switch(DST)                                                              \
  {                                                                        \
    case _: break;                                                         \
    case W: state->regs[r0] = dst; break;                                  \
    default: abort();                                                      \
  }                                                                        \
                                                                           \
  switch(CF)                                                               \
  {                                                                        \
    case _: break;                                                         \
    case W: state->cf = cf; break;                                         \
    default: abort();                                                      \
  }                                                                        \
                                                                           \
  switch(OF)                                                               \
  {                                                                        \
    case _: break;                                                         \
    case W: state->of = of; break;                                         \
    default: abort();                                                      \
  }                                                                        \
                                                                           \
  switch(PF)                                                               \
  {                                                                        \
    case _: break;                                                         \
    case W: state->pf = pf; break;                                         \
    case M: state->pf = (0x9669 >> ((dst ^ (dst >> 4)) & 0xf)) & 1; break; \
    default: abort();                                                      \
  }                                                                        \
                                                                           \
  switch(SF)                                                               \
  {                                                                        \
    case _: break;                                                         \
    case W: state->sf = sf; break;                                         \
    case M: state->sf = dst >> 31; break;                                  \
    default: abort();                                                      \
  }                                                                        \
                                                                           \
  switch(ZF)                                                               \
  {                                                                        \
    case _: break;                                                         \
    case W: state->zf = zf; break;                                         \
    case M: state->zf = !dst; break;                                       \
    default: abort();                                                      \
  }                                                                        \
}

// for simplicity, implementations of unary and nullary operations also take two arguments, which are to be ignored
#define INSN1(NAME, DST, CF, OF, PF, SF, ZF, CODE) INSN2(NAME, DST, CF, OF, PF, SF, ZF, CODE)
#define INSN0(NAME, DST, CF, OF, PF, SF, ZF, CODE) INSN2(NAME, DST, CF, OF, PF, SF, ZF, CODE)

#include "insns.def"

#undef INSN0
#undef INSN1
#undef INSN2

#undef _
#undef W
#undef M

#define INSN0(NAME, DST, CF, OF, PF, SF, ZF, CODE) {#NAME, insn_##NAME},
#define INSN1(NAME, DST, CF, OF, PF, SF, ZF, CODE)
#define INSN2(NAME, DST, CF, OF, PF, SF, ZF, CODE)
xo_instruction xo_insns0[] =
{
#include "insns.def"
};
#undef INSN0
#undef INSN1
#undef INSN2

#define INSN0(NAME, DST, CF, OF, PF, SF, ZF, CODE)
#define INSN1(NAME, DST, CF, OF, PF, SF, ZF, CODE) {#NAME, insn_##NAME},
#define INSN2(NAME, DST, CF, OF, PF, SF, ZF, CODE)
xo_instruction xo_insns1[] =
{
#include "insns.def"
};
#undef INSN0
#undef INSN1
#undef INSN2

#define INSN0(NAME, DST, CF, OF, PF, SF, ZF, CODE)
#define INSN1(NAME, DST, CF, OF, PF, SF, ZF, CODE)
#define INSN2(NAME, DST, CF, OF, PF, SF, ZF, CODE) {#NAME, insn_##NAME},
xo_instruction xo_insns2[] =
{
#include "insns.def"
};
#undef INSN0
#undef INSN1
#undef INSN2
