#include <stdio.h>
#include "equivalence_checker_c.h"
#include "insns.h"
#include "instruction.h"
#include "machine_state.h"
#include "program.h"
#include "register_set.h"

static xo_register pf_(xo_register a)
{
  // This code was taken from [Sean Eron Anderson's Bit Twiddling Hacks][1]
  // and modified slightly to yield a 1 for an *even* number of bits.
  // [1]: http://graphics.stanford.edu/~seander/bithacks.html#ParityParallel

  return ((0x9669 >> ((a ^ (a >> 4)) & 0xf)) & 0x1);
}

static xo_register sf_(xo_register a)
{
  return (a >> (XO_NUM_BITS-1));
}

static xo_register zf_(xo_register a)
{
  return !a;
}

static void insn_add_(xo_machine_state *st, size_t r0, size_t r1)
{
  xo_register tmp = st->regs[r0] + st->regs[r1];

  xo_machine_state_set_cf(st, tmp < st->regs[r0]);
  xo_machine_state_set_of(st, sf_(st->regs[r0]) == sf_(st->regs[r1]) && sf_(st->regs[r0]) != sf_(tmp));
  xo_machine_state_set_pf(st, pf_(tmp));
  xo_machine_state_set_sf(st, sf_(tmp));
  xo_machine_state_set_zf(st, zf_(tmp));

  st->regs[r0] = tmp;
}

static void insn_adc_(xo_machine_state *st, size_t r0, size_t r1)
{
  xo_register cf = xo_machine_state_get_cf(st);

  xo_register tmp = st->regs[r0] + st->regs[r1] + cf;

  xo_machine_state_set_cf(st, cf ? tmp <= st->regs[r0] : tmp < st->regs[r0]);
  xo_machine_state_set_of(st, sf_(st->regs[r0]) == sf_(st->regs[r1]) && sf_(st->regs[r0]) != sf_(tmp));
  xo_machine_state_set_pf(st, pf_(tmp));
  xo_machine_state_set_sf(st, sf_(tmp));
  xo_machine_state_set_zf(st, zf_(tmp));

  st->regs[r0] = tmp;
}

static void insn_sub_(xo_machine_state *st, size_t r0, size_t r1)
{
  xo_register tmp = st->regs[r0] - st->regs[r1];

  xo_machine_state_set_cf(st, tmp > st->regs[r0]);
  xo_machine_state_set_of(st, sf_(st->regs[r0]) != sf_(st->regs[r1]) && sf_(st->regs[r0]) != sf_(tmp));
  xo_machine_state_set_pf(st, pf_(tmp));
  xo_machine_state_set_sf(st, sf_(tmp));
  xo_machine_state_set_zf(st, zf_(tmp));

  st->regs[r0] = tmp;
}

static void insn_sbb_(xo_machine_state *st, size_t r0, size_t r1)
{
  xo_register cf = xo_machine_state_get_cf(st);

  xo_register tmp = st->regs[r0] - (st->regs[r1] + cf);

  xo_machine_state_set_cf(st, cf ? tmp >= st->regs[r0] : tmp > st->regs[r0]);
  xo_machine_state_set_of(st, sf_(st->regs[r0]) != sf_(st->regs[r1]) && sf_(st->regs[r0]) != sf_(tmp));
  xo_machine_state_set_pf(st, pf_(tmp));
  xo_machine_state_set_sf(st, sf_(tmp));
  xo_machine_state_set_zf(st, zf_(tmp));

  st->regs[r0] = tmp;
}

static void insn_cmp_(xo_machine_state *st, size_t r0, size_t r1)
{
  xo_register tmp = st->regs[r0] - st->regs[r1];

  xo_machine_state_set_cf(st, tmp > st->regs[r0]);
  xo_machine_state_set_of(st, sf_(st->regs[r0]) != sf_(st->regs[r1]) && sf_(st->regs[r0]) != sf_(tmp));
  xo_machine_state_set_pf(st, pf_(tmp));
  xo_machine_state_set_sf(st, sf_(tmp));
  xo_machine_state_set_zf(st, zf_(tmp));
}

static void insn_inc_(xo_machine_state *st, size_t r0, size_t r1)
{
  st->regs[r0] += 1;

  xo_machine_state_set_of(st, st->regs[r0] == 0x80000000);
  xo_machine_state_set_pf(st, pf_(st->regs[r0]));
  xo_machine_state_set_sf(st, sf_(st->regs[r0]));
  xo_machine_state_set_zf(st, zf_(st->regs[r0]));
}

static void insn_dec_(xo_machine_state *st, size_t r0, size_t r1)
{
  st->regs[r0] -= 1;

  xo_machine_state_set_of(st, st->regs[r0] == 0x7fffffff);
  xo_machine_state_set_pf(st, pf_(st->regs[r0]));
  xo_machine_state_set_sf(st, sf_(st->regs[r0]));
  xo_machine_state_set_zf(st, zf_(st->regs[r0]));
}

static void insn_neg_(xo_machine_state *st, size_t r0, size_t r1)
{
  st->regs[r0] = -st->regs[r0];

  xo_machine_state_set_cf(st, st->regs[r0] != 0);
  xo_machine_state_set_of(st, st->regs[r0] == 0x80000000);
  xo_machine_state_set_pf(st, pf_(st->regs[r0]));
  xo_machine_state_set_sf(st, sf_(st->regs[r0]));
  xo_machine_state_set_zf(st, zf_(st->regs[r0]));
}

static void insn_and_(xo_machine_state *st, size_t r0, size_t r1)
{
  st->regs[r0] &= st->regs[r1];

  xo_machine_state_set_cf(st, 0);
  xo_machine_state_set_of(st, 0);
  xo_machine_state_set_pf(st, pf_(st->regs[r0]));
  xo_machine_state_set_sf(st, sf_(st->regs[r0]));
  xo_machine_state_set_zf(st, zf_(st->regs[r0]));
}

static void insn_or_(xo_machine_state *st, size_t r0, size_t r1)
{
  st->regs[r0] |= st->regs[r1];

  xo_machine_state_set_cf(st, 0);
  xo_machine_state_set_of(st, 0);
  xo_machine_state_set_pf(st, pf_(st->regs[r0]));
  xo_machine_state_set_sf(st, sf_(st->regs[r0]));
  xo_machine_state_set_zf(st, zf_(st->regs[r0]));
}

static void insn_xor_(xo_machine_state *st, size_t r0, size_t r1)
{
  st->regs[r0] ^= st->regs[r1];

  xo_machine_state_set_cf(st, 0);
  xo_machine_state_set_of(st, 0);
  xo_machine_state_set_pf(st, pf_(st->regs[r0]));
  xo_machine_state_set_sf(st, sf_(st->regs[r0]));
  xo_machine_state_set_zf(st, zf_(st->regs[r0]));
}

static void insn_not_(xo_machine_state *st, size_t r0, size_t r1)
{
  st->regs[r0] = ~st->regs[r0];
}

static void insn_stc_(xo_machine_state *st, size_t r0, size_t r1)
{
  xo_machine_state_set_cf(st, 1);
}

static void insn_clc_(xo_machine_state *st, size_t r0, size_t r1)
{
  xo_machine_state_set_cf(st, 0);
}

static void insn_cmc_(xo_machine_state *st, size_t r0, size_t r1)
{
  xo_machine_state_set_cf(st, !xo_machine_state_get_cf(st));
}

static void insn_mov_(xo_machine_state *st, size_t r0, size_t r1)
{
  st->regs[r0] = st->regs[r1];
}

#define CMOV(NAME, COND) \
  static void insn_cmov##NAME##_(xo_machine_state *st, size_t r0, size_t r1) \
  { \
    if(COND) \
      st->regs[r0] = st->regs[r1]; \
  }

#define CF xo_machine_state_get_cf(st)
#define OF xo_machine_state_get_of(st)
#define PF xo_machine_state_get_pf(st)
#define SF xo_machine_state_get_sf(st)
#define ZF xo_machine_state_get_zf(st)

CMOV(c,  CF)
CMOV(o,  OF)
CMOV(p,  PF)
CMOV(s,  SF)
CMOV(z,  ZF)
CMOV(nc, !CF)
CMOV(no, !OF)
CMOV(np, !PF)
CMOV(ns, !SF)
CMOV(nz, !ZF)
CMOV(a,  !CF && !ZF)
CMOV(be, CF || ZF)
CMOV(g,  !ZF && SF == OF)
CMOV(ge, SF == OF)
CMOV(l,  SF != OF)
CMOV(le, ZF || SF != OF)

#undef CF
#undef OF
#undef PF
#undef SF
#undef ZF

#undef CMOV

typedef void (*c_impl_)(xo_machine_state *, size_t, size_t);

c_impl_ impl_for_insn_(const xo_instruction *insn)
{
  static c_impl_ impls[XO_NUM_INSNS];

  impls[XO_INSN_ADD] = insn_add_;
  impls[XO_INSN_ADC] = insn_adc_;
  impls[XO_INSN_SUB] = insn_sub_;
  impls[XO_INSN_SBB] = insn_sbb_;
  impls[XO_INSN_CMP] = insn_cmp_;
  impls[XO_INSN_INC] = insn_inc_;
  impls[XO_INSN_DEC] = insn_dec_;
  impls[XO_INSN_NEG] = insn_neg_;

  impls[XO_INSN_AND] = insn_and_;
  impls[XO_INSN_OR]  = insn_or_;
  impls[XO_INSN_XOR] = insn_xor_;
  impls[XO_INSN_NOT] = insn_not_;

  impls[XO_INSN_STC] = insn_stc_;
  impls[XO_INSN_CLC] = insn_clc_;
  impls[XO_INSN_CMC] = insn_cmc_;

  impls[XO_INSN_MOV] = insn_mov_;

  impls[XO_INSN_CMOVC]  = insn_cmovc_;
  impls[XO_INSN_CMOVO]  = insn_cmovo_;
  impls[XO_INSN_CMOVP]  = insn_cmovp_;
  impls[XO_INSN_CMOVS]  = insn_cmovs_;
  impls[XO_INSN_CMOVZ]  = insn_cmovz_;
  impls[XO_INSN_CMOVNC] = insn_cmovnc_;
  impls[XO_INSN_CMOVNO] = insn_cmovno_;
  impls[XO_INSN_CMOVNP] = insn_cmovnp_;
  impls[XO_INSN_CMOVNS] = insn_cmovns_;
  impls[XO_INSN_CMOVNZ] = insn_cmovnz_;
  impls[XO_INSN_CMOVA]  = insn_cmova_;
  impls[XO_INSN_CMOVBE] = insn_cmovbe_;
  impls[XO_INSN_CMOVG]  = insn_cmovg_;
  impls[XO_INSN_CMOVGE] = insn_cmovge_;
  impls[XO_INSN_CMOVL]  = insn_cmovl_;
  impls[XO_INSN_CMOVLE] = insn_cmovle_;

  return impls[insn->id];
}

void xo_equivalence_checker_c_run_program_on_state(const xo_program *prog, xo_machine_state *st)
{
  for(size_t i = 0; i < prog->num_invocations; ++i)
  {
    xo_invocation *inv = &prog->invocations[i];
    c_impl_ impl = impl_for_insn_(inv->insn);
    if(!impl)
    {
      fprintf(stderr, "unknown instruction: '%s'\n", inv->insn->name);
      return;
    }
    impl(st, inv->r0, inv->r1);
  }
}

static xo_register return_value_(const xo_program *prog, const xo_machine_state *st, size_t ro_index)
{
  xo_machine_state st_copy;
  xo_machine_state_copy(&st_copy, st);
  xo_equivalence_checker_c_run_program_on_state(prog, &st_copy);
  return st_copy.regs[ro_index];
}

bool xo_equivalence_checker_c_programs_equivalent_on_states(const xo_program *prog1, const xo_program *prog2, 
    size_t num_states, const xo_machine_state *states)
{
  xo_register_set ro_set_1, ro_set_2;

  xo_program_analyze(prog1, NULL, &ro_set_1);
  xo_program_analyze(prog2, NULL, &ro_set_2);

  if(ro_set_1 != ro_set_2)
    return false;

  size_t ro_index = xo_register_set_first_live_index(ro_set_1);

  for(size_t i = 0; i < num_states; ++i)
  {
    xo_register v1 = return_value_(prog1, &states[i], ro_index);
    xo_register v2 = return_value_(prog2, &states[i], ro_index);

    if(v1 != v2)
      return false;
  }

  return true;
}

