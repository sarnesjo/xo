#include <stdlib.h>
#include "insns.h"

#define DST (1 << 0)
#define SRC (1 << 1)

#define CF (1 << 0)
#define OF (1 << 1)
#define PF (1 << 2)
#define SF (1 << 3)
#define ZF (1 << 4)

// TODO: what to do about code? define here or elsewhere?
#define INSN(NAME, ARITY, IREGS, OREGS, IFLAGS, OFLAGS, DFLAGS, CODE) void insn_##NAME(xo_machine_state *s, size_t r0, size_t r1) {}
#include "insns.def"
#undef INSN

#define INSN(NAME, ARITY, IREGS, OREGS, IFLAGS, OFLAGS, DFLAGS, CODE) {#NAME, ARITY, IREGS, OREGS, IFLAGS, OFLAGS, DFLAGS, insn_##NAME},
xo_instruction xo_insns[] =
{
#include "insns.def"
};
#undef INSN
