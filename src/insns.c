#include "flag_set.h"
#include "insns.h"

// TODO: implement mul, imul? cf/of: !!(result & 0xffff0000)
// TODO: implement neg
// TODO: implement lea
// TODO: implement rotate and shift insns

#define R0 (1 << 0)
#define R1 (1 << 1)

#define CF XO_FLAG_SET_CF
#define OF XO_FLAG_SET_OF
#define PF XO_FLAG_SET_PF
#define SF XO_FLAG_SET_SF
#define ZF XO_FLAG_SET_ZF

xo_instruction xo_insns[] =
{
  // binary arithmetic
  {XO_INSN_ADD, "add", 2, R0|R1, R0, 0,  CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {XO_INSN_ADC, "adc", 2, R0|R1, R0, CF, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {XO_INSN_SUB, "sub", 2, R0|R1, R0, 0,  CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {XO_INSN_SBB, "sbb", 2, R0|R1, R0, CF, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {XO_INSN_CMP, "cmp", 2, R0|R1, 0,  0,  CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {XO_INSN_INC, "inc", 1, R0,    R0, 0,  OF|PF|SF|ZF,    OF|PF|SF|ZF},
  {XO_INSN_DEC, "dec", 1, R0,    R0, 0,  OF|PF|SF|ZF,    OF|PF|SF|ZF},

  // logic
  {XO_INSN_AND, "and", 2, R0|R1, R0, 0, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {XO_INSN_OR,  "or",  2, R0|R1, R0, 0, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {XO_INSN_XOR, "xor", 2, R0|R1, R0, 0, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {XO_INSN_NOT, "not", 1, R0,    R0, 0, 0,              0},

  // flag control
  {XO_INSN_STC, "stc", 0, 0, 0, 0,  CF, CF},
  {XO_INSN_CLC, "clc", 0, 0, 0, 0,  CF, CF},
  {XO_INSN_CMC, "cmc", 0, 0, 0, CF, CF, CF},

  // move
  {XO_INSN_MOV, "mov", 2, R1, R0, 0, 0, 0}, // imm

  // conditional move
  {XO_INSN_CMOVC,  "cmovc",  2, R0|R1, R0, CF,       0, 0},
  {XO_INSN_CMOVO,  "cmovo",  2, R0|R1, R0, OF,       0, 0},
  {XO_INSN_CMOVP,  "cmovp",  2, R0|R1, R0, PF,       0, 0},
  {XO_INSN_CMOVS,  "cmovs",  2, R0|R1, R0, SF,       0, 0},
  {XO_INSN_CMOVZ,  "cmovz",  2, R0|R1, R0, ZF,       0, 0},
  {XO_INSN_CMOVNC, "cmovnc", 2, R0|R1, R0, CF,       0, 0},
  {XO_INSN_CMOVNO, "cmovno", 2, R0|R1, R0, OF,       0, 0},
  {XO_INSN_CMOVNP, "cmovnp", 2, R0|R1, R0, PF,       0, 0},
  {XO_INSN_CMOVNS, "cmovns", 2, R0|R1, R0, SF,       0, 0},
  {XO_INSN_CMOVNZ, "cmovnz", 2, R0|R1, R0, ZF,       0, 0},
  {XO_INSN_CMOVA,  "cmova",  2, R0|R1, R0, CF|ZF,    0, 0}, // above, unsigned
  {XO_INSN_CMOVBE, "cmovbe", 2, R0|R1, R0, CF|ZF,    0, 0}, // below or equal, unsigned
  {XO_INSN_CMOVG,  "cmovg",  2, R0|R1, R0, OF|SF|ZF, 0, 0}, // greater, signed
  {XO_INSN_CMOVGE, "cmovge", 2, R0|R1, R0, OF|SF,    0, 0}, // greater or equal, signed
  {XO_INSN_CMOVL,  "cmovl",  2, R0|R1, R0, OF|SF,    0, 0}, // less, signed
  {XO_INSN_CMOVLE, "cmovle", 2, R0|R1, R0, OF|SF|ZF, 0, 0}, // less or equal, signed

  // other cmov forms:
  // ae=nc, b=c, na=be, nae=c, nb=nc, nbe=a
  // ng=le, nge=l, nl=ge, nle=g
  // e=z, ne=zf
  // pe=p, po=np
};

#undef R0
#undef R1

#undef CF
#undef OF
#undef PF
#undef SF
#undef ZF
