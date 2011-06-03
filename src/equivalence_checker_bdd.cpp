#include <bdd.h>
#include <stdio.h>
#include "equivalence_checker_bdd.h"

// TODO: much of this code assumes that XO_NUM_BITS == 32

// converting BDDs to and from register and flag values

// every value in a should be bdd_true or bdd_false
static xo_register bdd_to_register_(const bdd a[XO_NUM_BITS])
{
  xo_register r = 0;
  for(int i = 0; i < XO_NUM_BITS; ++i)
    if(a[i] == bdd_true())
      r |= (1 << i);
  return r;
}

static void bdd_from_register_(bdd a[XO_NUM_BITS], xo_register r)
{
  for(int i = 0; i < XO_NUM_BITS; ++i)
    a[i] = (r & (1 << i)) ? bdd_true() : bdd_false();
}

// every value in a should be bdd_true or bdd_false
static xo_flag_set bdd_to_flag_set_(const bdd a[XO_NUM_FLAGS])
{
  xo_flag_set s = 0;
  for(int i = 0; i < XO_NUM_FLAGS; ++i)
    if(a[i] == bdd_true())
      s |= (1 << i);
  return s;
}

static void bdd_from_flag_set_(bdd a[XO_NUM_FLAGS], xo_flag_set s)
{
  for(int i = 0; i < XO_NUM_FLAGS; ++i)
    a[i] = (s & (1 << i)) ? bdd_true() : bdd_false();
}

// parity, sign and zero flag

static bdd pf_(const bdd a[XO_NUM_BITS])
{
  return !(a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[4] ^ a[5] ^ a[6] ^ a[7]);
}

static bdd sf_(const bdd a[XO_NUM_BITS])
{
  return a[31];
}

static bdd zf_(const bdd a[XO_NUM_BITS])
{
  return !(a[ 0] | a[ 1] | a[ 2] | a[ 3] | a[ 4] | a[ 5] | a[ 6] | a[ 7] | a[ 8] | a[ 9]
         | a[10] | a[11] | a[12] | a[13] | a[14] | a[15] | a[16] | a[17] | a[18] | a[19]
         | a[20] | a[21] | a[22] | a[23] | a[24] | a[25] | a[26] | a[27] | a[28] | a[29]
         | a[30] | a[31]);
}

// insn impls

// TODO: there is a lot of code duplication between add/adc/inc, sub/sbb/dec

static void insn_add_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  bdd c[XO_NUM_BITS];

  c[0] = r0[0] & r1[0];
  r0[0] = r0[0] ^ r1[0];

  for(int i = 1; i < XO_NUM_BITS; ++i)
  {
    c[i] = r0[i] & r1[i] | r0[i] & c[i-1] | r1[i] & c[i-1];
    r0[i] = r0[i] ^ r1[i] ^ c[i-1];
  }

  f[0] = c[31];
  f[1] = c[30] ^ c[31];
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_adc_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  bdd c[XO_NUM_BITS];

  c[0] = r0[0] & r1[0] | r0[0] & f[0] | r1[0] & f[0];
  r0[0] = r0[0] ^ r1[0] ^ f[0];

  for(int i = 1; i < XO_NUM_BITS; ++i)
  {
    c[i] = r0[i] & r1[i] | r0[i] & c[i-1] | r1[i] & c[i-1];
    r0[i] = r0[i] ^ r1[i] ^ c[i-1];
  }

  f[0] = c[31];
  f[1] = c[30] ^ c[31];
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_sub_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  bdd c[XO_NUM_BITS];

  c[0] = !r0[0] & r1[0];
  r0[0] = r0[0] ^ r1[0];

  for(int i = 1; i < XO_NUM_BITS; ++i)
  {
    c[i] = !r0[i] & r1[i] | !r0[i] & c[i-1] | r1[i] & c[i-1];
    r0[i] = r0[i] ^ r1[i] ^ c[i-1];
  }

  f[0] = c[31];
  f[1] = c[30] ^ c[31];
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_sbb_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  bdd c[XO_NUM_BITS];

  c[0] = !r0[0] & r1[0] | !r0[0] & f[0] | r1[0] & f[0];
  r0[0] = r0[0] ^ r1[0] ^ f[0];

  for(int i = 1; i < XO_NUM_BITS; ++i)
  {
    c[i] = !r0[i] & r1[i] | !r0[i] & c[i-1] | r1[i] & c[i-1];
    r0[i] = r0[i] ^ r1[i] ^ c[i-1];
  }

  f[0] = c[31];
  f[1] = c[30] ^ c[31];
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_cmp_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  bdd c[XO_NUM_BITS];
  bdd tmp[XO_NUM_BITS];

  c[0] = !r0[0] & r1[0];
  tmp[0] = r0[0] ^ r1[0];

  for(int i = 1; i < XO_NUM_BITS; ++i)
  {
    c[i] = !r0[i] & r1[i] | !r0[i] & c[i-1] | r1[i] & c[i-1];
    tmp[i] = r0[i] ^ r1[i] ^ c[i-1];
  }

  f[0] = c[31];
  f[1] = c[30] ^ c[31];
  f[2] = pf_(tmp);
  f[3] = sf_(tmp);
  f[4] = zf_(tmp);
}

static void insn_inc_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  bdd c[XO_NUM_BITS];

  c[0] = r0[0];
  r0[0] = !r0[0];

  for(int i = 1; i < XO_NUM_BITS; ++i)
  {
    c[i] = r0[i] & c[i-1];
    r0[i] = r0[i] ^ c[i-1];
  }

  f[1] = c[30] ^ c[31];
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_dec_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  bdd c[XO_NUM_BITS];

  c[0] = !r0[0];
  r0[0] = !r0[0];

  for(int i = 1; i < XO_NUM_BITS; ++i)
  {
    c[i] = !r0[i] & c[i-1];
    r0[i] = r0[i] ^ c[i-1];
  }

  f[1] = c[30] ^ c[31];
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_and_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  for(int i = 0; i < XO_NUM_BITS; ++i)
    r0[i] = r0[i] & r1[i];

  f[0] = bdd_false();
  f[1] = bdd_false();
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_or_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  for(int i = 0; i < XO_NUM_BITS; ++i)
    r0[i] = r0[i] | r1[i];

  f[0] = bdd_false();
  f[1] = bdd_false();
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_xor_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  for(int i = 0; i < XO_NUM_BITS; ++i)
    r0[i] = r0[i] ^ r1[i];

  f[0] = bdd_false();
  f[1] = bdd_false();
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_not_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  for(int i = 0; i < XO_NUM_BITS; ++i)
    r0[i] = !r0[i];
}

static void insn_stc_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  f[0] = bdd_true();
}

static void insn_clc_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  f[0] = bdd_false();
}

static void insn_cmc_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  f[0] = !f[0];
}

static void insn_mov_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  for(int i = 0; i < XO_NUM_BITS; ++i)
    r0[i] = r1[i];
}

#define CMOV(NAME, COND) \
  static void insn_cmov##NAME##_(bdd r0[XO_NUM_BITS], bdd r1[XO_NUM_BITS], bdd f[XO_NUM_FLAGS]) \
  { \
    bdd cond = (COND); \
    for(int i = 0; i < XO_NUM_BITS; ++i) \
      r0[i] = !cond & r0[i] | cond & r1[i]; \
  }

CMOV(c, f[0])
CMOV(o, f[1])
CMOV(p, f[2])
CMOV(s, f[3])
CMOV(z, f[4])

CMOV(nc, !f[0])
CMOV(no, !f[1])
CMOV(np, !f[2])
CMOV(ns, !f[3])
CMOV(nz, !f[4])

CMOV(be,   f[0] | f[4])
CMOV(a,  !(f[0] | f[4]))

CMOV(le,   f[4] | f[3] ^ f[1])
CMOV(g,  !(f[4] | f[3] ^ f[1]))

CMOV(l,    f[3] ^ f[1])
CMOV(ge, !(f[3] ^ f[1]))

#undef CMOV

typedef void (*bdd_impl_)(bdd *, bdd *, bdd *);

bdd_impl_ impl_for_insn_(const xo_instruction *insn)
{
  static bdd_impl_ impls[XO_NUM_INSNS];

  impls[XO_INSN_ADD] = insn_add_;
  impls[XO_INSN_ADC] = insn_adc_;
  impls[XO_INSN_SUB] = insn_sub_;
  impls[XO_INSN_SBB] = insn_sbb_;
  impls[XO_INSN_CMP] = insn_cmp_;
  impls[XO_INSN_INC] = insn_inc_;
  impls[XO_INSN_DEC] = insn_dec_;

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

static void evaluate_program_(const xo_program *prog, bdd r[XO_NUM_REGISTERS][XO_NUM_BITS], bdd f[XO_NUM_FLAGS])
{
  for(size_t i = 0; i < prog->num_invocations; ++i)
  {
    xo_invocation *inv = &prog->invocations[i];
    bdd_impl_ impl = impl_for_insn_(inv->insn);
    if(!impl)
    {
      fprintf(stderr, "unknown instruction: '%s'\n", inv->insn->name);
      return;
    }
    impl(r[inv->r0], r[inv->r1], f);
  }
}

void xo_equivalence_checker_bdd_evaluate_program_on_state(const xo_program *prog, xo_machine_state *st)
{
  bdd_init(1000, 100); // TODO: good values?

  bdd r[XO_NUM_REGISTERS][XO_NUM_BITS], f[XO_NUM_FLAGS];

  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    bdd_from_register_(r[i], st->regs[i]);
  bdd_from_flag_set_(f, st->flags);

  evaluate_program_(prog, r, f);

  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    st->regs[i] = bdd_to_register_(r[i]);
  st->flags = bdd_to_flag_set_(f);

  bdd_done();
}

bool xo_equivalence_checker_bdd_programs_equivalent(const xo_program *prog1, const xo_program *prog2)
{
  xo_register_set ro_set_1, ro_set_2;

  xo_program_analyze(prog1, NULL, &ro_set_1);
  xo_program_analyze(prog2, NULL, &ro_set_2);

  if(ro_set_1 != ro_set_2)
    return false;

  bdd_init(1000000, 100000); // TODO: good values?
  bdd_setvarnum(XO_NUM_REGISTERS*XO_NUM_BITS);

  bdd r[2][XO_NUM_REGISTERS][XO_NUM_BITS], f[2][XO_NUM_FLAGS];

  int index = 0;
  for(int bi = 0; bi < XO_NUM_BITS; ++bi)
    for(int ri = 0; ri < XO_NUM_REGISTERS; ++ri)
      r[0][ri][bi] = r[1][ri][bi] = bdd_ithvar(index++);

  bdd_from_flag_set_(f[0], 0);
  bdd_from_flag_set_(f[1], 0);

  evaluate_program_(prog1, r[0], f[0]);
  evaluate_program_(prog2, r[1], f[1]);

  size_t ro_index = xo_register_set_first_live_index(ro_set_1);
  bool equiv = true;

  for(size_t i = 0; i < XO_NUM_BITS; ++i)
  {
    if(r[0][ro_index][i] != r[1][ro_index][i])
    {
      equiv = false;
      break;
    }
  }

  bdd_done();

  return equiv;
}
