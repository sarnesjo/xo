#include <stdlib.h>
#include "flag_set.h"
#include "insns.h"

#define DST (1 << 0)
#define SRC (1 << 1)

#define CF XO_FLAG_SET_CF
#define OF XO_FLAG_SET_OF
#define PF XO_FLAG_SET_PF
#define SF XO_FLAG_SET_SF
#define ZF XO_FLAG_SET_ZF

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

#undef DST
#undef SRC

#undef CF
#undef OF
#undef PF
#undef SF
#undef ZF
