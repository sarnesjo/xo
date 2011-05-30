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
  {"add", 2, R0|R1, R0, 0,  CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {"adc", 2, R0|R1, R0, CF, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {"sub", 2, R0|R1, R0, 0,  CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {"sbb", 2, R0|R1, R0, CF, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {"cmp", 2, R0|R1, 0,  0,  CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {"inc", 1, R0,    R0, 0,  OF|PF|SF|ZF,    OF|PF|SF|ZF},
  {"dec", 1, R0,    R0, 0,  OF|PF|SF|ZF,    OF|PF|SF|ZF},

  // logic
  {"and", 2, R0|R1, R0, 0, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {"or",  2, R0|R1, R0, 0, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {"xor", 2, R0|R1, R0, 0, CF|OF|PF|SF|ZF, CF|OF|PF|SF|ZF}, // imm
  {"not", 1, R0,    R0, 0, 0,              0},

  // flag control
  {"stc", 0, 0, 0, 0,  CF, CF},
  {"clc", 0, 0, 0, 0,  CF, CF},
  {"cmc", 0, 0, 0, CF, CF, CF},

  // move
  {"mov", 2, R1, R0, 0, 0, 0}, // imm

  // conditional move
  {"cmovc",  2, R0|R1, R0, CF,       0, 0},
  {"cmovo",  2, R0|R1, R0, OF,       0, 0},
  {"cmovp",  2, R0|R1, R0, PF,       0, 0},
  {"cmovs",  2, R0|R1, R0, SF,       0, 0},
  {"cmovz",  2, R0|R1, R0, ZF,       0, 0},
  {"cmovnc", 2, R0|R1, R0, CF,       0, 0},
  {"cmovno", 2, R0|R1, R0, OF,       0, 0},
  {"cmovnp", 2, R0|R1, R0, PF,       0, 0},
  {"cmovns", 2, R0|R1, R0, SF,       0, 0},
  {"cmovnz", 2, R0|R1, R0, ZF,       0, 0},
  {"cmova",  2, R0|R1, R0, CF|ZF,    0, 0}, // above, unsigned
  {"cmovbe", 2, R0|R1, R0, CF|ZF,    0, 0}, // below or equal, unsigned
  {"cmovg",  2, R0|R1, R0, OF|SF|ZF, 0, 0}, // greater, signed
  {"cmovge", 2, R0|R1, R0, OF|SF,    0, 0}, // greater or equal, signed
  {"cmovl",  2, R0|R1, R0, OF|SF,    0, 0}, // less, signed
  {"cmovle", 2, R0|R1, R0, OF|SF|ZF, 0, 0}, // less or equal, signed

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
