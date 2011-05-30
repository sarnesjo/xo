#include <bdd.h>
#include "equivalence_checker_bdd.h"

// TODO: use defs from types.h?
#define NUM_BITS 32
#define NUM_FLAGS 5

// converting BDDs to and from register and flag values

static uint32_t bdd_to_uint32_(const bdd a[NUM_BITS])
{
  uint32_t u = 0;
  for(int i = 0; i < NUM_BITS; ++i)
    if(!(a[i] == bdd_true() || a[i] == bdd_false()))
      abort(); // TODO: warn if neither bdd_true nor bdd_false?
    else if(a[i] == bdd_true())
      u |= (1 << i);
  return u;
}

static void bdd_from_uint32_(bdd a[NUM_BITS], uint32_t u)
{
  for(int i = 0; i < NUM_BITS; ++i)
    a[i] = (u & (1 << i)) ? bdd_true() : bdd_false();
}

static xo_flag_set bdd_to_flag_set_(const bdd a[NUM_FLAGS])
{
  xo_flag_set s = 0;
  for(int i = 0; i < NUM_FLAGS; ++i)
    if(!(a[i] == bdd_true() || a[i] == bdd_false()))
      abort(); // TODO: warn if neither bdd_true nor bdd_false?
    else if(a[i] == bdd_true())
      s |= (1 << i);
  return s;
}

static void bdd_from_flag_set_(bdd a[NUM_FLAGS], xo_flag_set s)
{
  for(int i = 0; i < NUM_FLAGS; ++i)
    a[i] = (s & (1 << i)) ? bdd_true() : bdd_false();
}

// parity, sign and zero flag
// TODO: assumes that NUM_BITS == 32

static bdd pf_(const bdd a[NUM_BITS])
{
  return !(a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[4] ^ a[5] ^ a[6] ^ a[7]);
}

static bdd sf_(const bdd a[NUM_BITS])
{
  return a[31];
}

static bdd zf_(const bdd a[NUM_BITS])
{
  return !(a[ 0] | a[ 1] | a[ 2] | a[ 3] | a[ 4] | a[ 5] | a[ 6] | a[ 7] | a[ 8] | a[ 9]
         | a[10] | a[11] | a[12] | a[13] | a[14] | a[15] | a[16] | a[17] | a[18] | a[19]
         | a[20] | a[21] | a[22] | a[23] | a[24] | a[25] | a[26] | a[27] | a[28] | a[29]
         | a[30] | a[31]);
}

// insn impls

// TODO: there is a lot of code duplication between add/adc/inc, sub/sbb/dec

static void insn_add_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  bdd c[NUM_BITS];

  c[0] = r0[0] & r1[0];
  r0[0] = r0[0] ^ r1[0];

  for(int i = 1; i < NUM_BITS; ++i)
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

static void insn_adc_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  bdd c[NUM_BITS];

  c[0] = r0[0] & r1[0] | r0[0] & f[0] | r1[0] & f[0];
  r0[0] = r0[0] ^ r1[0] ^ f[0];

  for(int i = 1; i < NUM_BITS; ++i)
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

static void insn_sub_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  bdd c[NUM_BITS];

  c[0] = !r0[0] & r1[0];
  r0[0] = r0[0] ^ r1[0];

  for(int i = 1; i < NUM_BITS; ++i)
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

static void insn_sbb_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  bdd c[NUM_BITS];

  c[0] = !r0[0] & r1[0] | !r0[0] & f[0] | r1[0] & f[0];
  r0[0] = r0[0] ^ r1[0] ^ f[0];

  for(int i = 1; i < NUM_BITS; ++i)
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

static void insn_cmp_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  bdd c[NUM_BITS];
  bdd tmp[NUM_BITS];

  c[0] = !r0[0] & r1[0];
  tmp[0] = r0[0] ^ r1[0];

  for(int i = 1; i < NUM_BITS; ++i)
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

static void insn_inc_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  bdd c[NUM_BITS];

  c[0] = r0[0] & bdd_true();
  r0[0] = r0[0] ^ bdd_true();

  for(int i = 1; i < NUM_BITS; ++i)
  {
    c[i] = r0[i] & bdd_false() | r0[i] & c[i-1] | bdd_false() & c[i-1];
    r0[i] = r0[i] ^ bdd_false() ^ c[i-1];
  }

  f[1] = c[30] ^ c[31];
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_dec_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  bdd c[NUM_BITS];

  c[0] = !r0[0] & bdd_true();
  r0[0] = r0[0] ^ bdd_true();

  for(int i = 1; i < NUM_BITS; ++i)
  {
    c[i] = !r0[i] & bdd_false() | !r0[i] & c[i-1] | bdd_false() & c[i-1];
    r0[i] = r0[i] ^ bdd_false() ^ c[i-1];
  }

  f[1] = c[30] ^ c[31];
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_and_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  for(int i = 0; i < NUM_BITS; ++i)
    r0[i] = r0[i] & r1[i];

  f[0] = bdd_false();
  f[1] = bdd_false();
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_or_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  for(int i = 0; i < NUM_BITS; ++i)
    r0[i] = r0[i] | r1[i];

  f[0] = bdd_false();
  f[1] = bdd_false();
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_xor_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  for(int i = 0; i < NUM_BITS; ++i)
    r0[i] = r0[i] ^ r1[i];

  f[0] = bdd_false();
  f[1] = bdd_false();
  f[2] = pf_(r0);
  f[3] = sf_(r0);
  f[4] = zf_(r0);
}

static void insn_not_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  for(int i = 0; i < NUM_BITS; ++i)
    r0[i] = !r0[i];
}

static void insn_stc_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  f[0] = bdd_true();
}

static void insn_clc_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  f[0] = bdd_false();
}

static void insn_cmc_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  f[0] = !f[0];
}

static void insn_mov_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS])
{
  for(int i = 0; i < NUM_BITS; ++i)
    r0[i] = r1[i];
}

#define CMOV(NAME, COND) \
  static void insn_cmov##NAME##_(bdd r0[NUM_BITS], bdd r1[NUM_BITS], bdd f[NUM_FLAGS]) \
  { \
    bdd cond = (COND); \
    for(int i = 0; i < NUM_BITS; ++i) \
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

static void evaluate_program_(const xo_program *prog, bdd r[XO_NUM_REGISTERS][NUM_BITS], bdd f[NUM_FLAGS])
{
  // TODO: better way to look up insns...

  for(size_t i = 0; i < prog->num_invocations; ++i)
  {
    xo_invocation *inv = &prog->invocations[i];
    void (*impl)(bdd *, bdd *, bdd*) = NULL;

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

    impl(r[inv->r0], r[inv->r1], f);
  }
}

void xo_equivalence_checker_bdd_evaluate_program_on_state(const xo_program *prog, xo_machine_state *st)
{
  bdd_init(1000, 100); // TODO: good values?

  bdd r[XO_NUM_REGISTERS][NUM_BITS], f[NUM_FLAGS];

  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    bdd_from_uint32_(r[i], st->regs[i]);
  bdd_from_flag_set_(f, st->flags);

  evaluate_program_(prog, r, f);

  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    st->regs[i] = bdd_to_uint32_(r[i]);
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
  bdd_setvarnum(XO_NUM_REGISTERS*NUM_BITS);

  bdd r[2][XO_NUM_REGISTERS][NUM_BITS], f[2][NUM_FLAGS];

  int index = 0;
  for(int bi = 0; bi < NUM_BITS; ++bi)
    for(int ri = 0; ri < XO_NUM_REGISTERS; ++ri)
      r[0][ri][bi] = r[1][ri][bi] = bdd_ithvar(index++);

  bdd_from_flag_set_(f[0], 0);
  bdd_from_flag_set_(f[1], 0);

  evaluate_program_(prog1, r[0], f[0]);
  evaluate_program_(prog2, r[1], f[1]);

  size_t ro_index = xo_register_set_first_live_index(ro_set_1);
  bool equiv = true;

  for(size_t i = 0; i < NUM_BITS; ++i)
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
