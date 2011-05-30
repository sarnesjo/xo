#include "equivalence_checker_c.h"
#include "instruction.h"
#include "machine_state.h"

static uint32_t pf_(uint32_t a)
{
  // This code was taken from [Sean Eron Anderson's Bit Twiddling Hacks][1]
  // and modified slightly to yield a 1 for an *even* number of bits.
  // [1]: http://graphics.stanford.edu/~seander/bithacks.html#ParityParallel

  return ((0x9669 >> ((a ^ (a >> 4)) & 0xf)) & 0x1);
}

static uint32_t sf_(uint32_t a)
{
  return (a >> 31);
}

static uint32_t zf_(uint32_t a)
{
  return !a;
}

static void insn_add_(xo_machine_state *st, size_t r0, size_t r1)
{
  uint32_t tmp = st->regs[r0] + st->regs[r1];

  xo_machine_state_set_cf(st, tmp < st->regs[r0]);
  xo_machine_state_set_of(st, sf_(st->regs[r0]) == sf_(st->regs[r1]) && sf_(st->regs[r0]) != sf_(tmp));
  xo_machine_state_set_pf(st, pf_(tmp));
  xo_machine_state_set_sf(st, sf_(tmp));
  xo_machine_state_set_zf(st, zf_(tmp));

  st->regs[r0] = tmp;
}

static void insn_adc_(xo_machine_state *st, size_t r0, size_t r1)
{
  uint32_t cf = xo_machine_state_get_cf(st);

  uint32_t tmp = st->regs[r0] + st->regs[r1] + cf;

  xo_machine_state_set_cf(st, cf ? tmp <= st->regs[r0] : tmp < st->regs[r0]);
  xo_machine_state_set_of(st, sf_(st->regs[r0]) == sf_(st->regs[r1]) && sf_(st->regs[r0]) != sf_(tmp));
  xo_machine_state_set_pf(st, pf_(tmp));
  xo_machine_state_set_sf(st, sf_(tmp));
  xo_machine_state_set_zf(st, zf_(tmp));

  st->regs[r0] = tmp;
}

static void insn_sub_(xo_machine_state *st, size_t r0, size_t r1)
{
  uint32_t tmp = st->regs[r0] - st->regs[r1];

  xo_machine_state_set_cf(st, tmp > st->regs[r0]);
  xo_machine_state_set_of(st, sf_(st->regs[r0]) != sf_(st->regs[r1]) && sf_(st->regs[r0]) != sf_(tmp));
  xo_machine_state_set_pf(st, pf_(tmp));
  xo_machine_state_set_sf(st, sf_(tmp));
  xo_machine_state_set_zf(st, zf_(tmp));

  st->regs[r0] = tmp;
}

static void insn_sbb_(xo_machine_state *st, size_t r0, size_t r1)
{
  uint32_t cf = xo_machine_state_get_cf(st);

  uint32_t tmp = st->regs[r0] - (st->regs[r1] + cf);

  xo_machine_state_set_cf(st, cf ? tmp >= st->regs[r0] : tmp > st->regs[r0]);
  xo_machine_state_set_of(st, sf_(st->regs[r0]) != sf_(st->regs[r1]) && sf_(st->regs[r0]) != sf_(tmp));
  xo_machine_state_set_pf(st, pf_(tmp));
  xo_machine_state_set_sf(st, sf_(tmp));
  xo_machine_state_set_zf(st, zf_(tmp));

  st->regs[r0] = tmp;
}

static void insn_cmp_(xo_machine_state *st, size_t r0, size_t r1)
{
  uint32_t tmp = st->regs[r0] - st->regs[r1];

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

void xo_equivalence_checker_c_run_program_on_state(const xo_program *prog, xo_machine_state *st)
{
  // TODO: better way to look up insns...

  for(size_t i = 0; i < prog->num_invocations; ++i)
  {
    xo_invocation *inv = &prog->invocations[i];
    void (*impl)(xo_machine_state *, size_t, size_t) = NULL;

    if(inv->insn == xo_instruction_get("add"))
      impl = insn_add_;
    else if(inv->insn == xo_instruction_get("adc"))
      impl = insn_adc_;
    else if(inv->insn == xo_instruction_get("sub"))
      impl = insn_sub_;
    else if(inv->insn == xo_instruction_get("sbb"))
      impl = insn_sbb_;
    else if(inv->insn == xo_instruction_get("cmp"))
      impl = insn_cmp_;
    else if(inv->insn == xo_instruction_get("inc"))
      impl = insn_inc_;
    else if(inv->insn == xo_instruction_get("dec"))
      impl = insn_dec_;

    else if(inv->insn == xo_instruction_get("and"))
      impl = insn_and_;
    else if(inv->insn == xo_instruction_get("or"))
      impl = insn_or_;
    else if(inv->insn == xo_instruction_get("xor"))
      impl = insn_xor_;
    else if(inv->insn == xo_instruction_get("not"))
      impl = insn_not_;

    else if(inv->insn == xo_instruction_get("stc"))
      impl = insn_stc_;
    else if(inv->insn == xo_instruction_get("clc"))
      impl = insn_clc_;
    else if(inv->insn == xo_instruction_get("cmc"))
      impl = insn_cmc_;

    else if(inv->insn == xo_instruction_get("mov"))
      impl = insn_mov_;

    else if(inv->insn == xo_instruction_get("cmovc"))
      impl = insn_cmovc_;
    else if(inv->insn == xo_instruction_get("cmovo"))
      impl = insn_cmovo_;
    else if(inv->insn == xo_instruction_get("cmovp"))
      impl = insn_cmovp_;
    else if(inv->insn == xo_instruction_get("cmovs"))
      impl = insn_cmovs_;
    else if(inv->insn == xo_instruction_get("cmovz"))
      impl = insn_cmovz_;

    else if(inv->insn == xo_instruction_get("cmovnc"))
      impl = insn_cmovnc_;
    else if(inv->insn == xo_instruction_get("cmovno"))
      impl = insn_cmovno_;
    else if(inv->insn == xo_instruction_get("cmovnp"))
      impl = insn_cmovnp_;
    else if(inv->insn == xo_instruction_get("cmovns"))
      impl = insn_cmovns_;
    else if(inv->insn == xo_instruction_get("cmovnz"))
      impl = insn_cmovnz_;

    else if(inv->insn == xo_instruction_get("cmova"))
      impl = insn_cmova_;
    else if(inv->insn == xo_instruction_get("cmovbe"))
      impl = insn_cmovbe_;
    else if(inv->insn == xo_instruction_get("cmovg"))
      impl = insn_cmovg_;
    else if(inv->insn == xo_instruction_get("cmovge"))
      impl = insn_cmovge_;
    else if(inv->insn == xo_instruction_get("cmovl"))
      impl = insn_cmovl_;
    else if(inv->insn == xo_instruction_get("cmovle"))
      impl = insn_cmovle_;

    impl(st, inv->r0, inv->r1);
  }
}

static uint32_t return_value_(const xo_program *prog, const xo_machine_state *st, size_t ro_index)
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
    return false; // TODO: signal error?

  size_t ro_index = xo_register_set_first_live_index(ro_set_1);

  for(size_t i = 0; i < num_states; ++i)
  {
    uint32_t v1 = return_value_(prog1, &states[i], ro_index);
    uint32_t v2 = return_value_(prog2, &states[i], ro_index);

    if(v1 != v2)
      return false;
  }

  return true;
}

