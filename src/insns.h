#ifndef XO_INSNS_H
#define XO_INSNS_H

#include "types.h"

enum
{
  XO_INSN_ADD,
  XO_INSN_ADC,
  XO_INSN_SUB,
  XO_INSN_SBB,
  XO_INSN_CMP,
  XO_INSN_INC,
  XO_INSN_DEC,
  XO_INSN_NEG,

  XO_INSN_AND,
  XO_INSN_OR,
  XO_INSN_XOR,
  XO_INSN_NOT,

  XO_INSN_STC,
  XO_INSN_CLC,
  XO_INSN_CMC,

  XO_INSN_MOV,

  XO_INSN_CMOVC,
  XO_INSN_CMOVO,
  XO_INSN_CMOVP,
  XO_INSN_CMOVS,
  XO_INSN_CMOVZ,
  XO_INSN_CMOVNC,
  XO_INSN_CMOVNO,
  XO_INSN_CMOVNP,
  XO_INSN_CMOVNS,
  XO_INSN_CMOVNZ,
  XO_INSN_CMOVA,
  XO_INSN_CMOVBE,
  XO_INSN_CMOVG,
  XO_INSN_CMOVGE,
  XO_INSN_CMOVL,
  XO_INSN_CMOVLE,

  XO_NUM_INSNS
};

#define XO_INSN_ID_IS_CMOV(INSN_ID) ((INSN_ID) >= XO_INSN_CMOVC && (INSN_ID) <= XO_INSN_CMOVLE)

extern xo_instruction xo_insns[XO_NUM_INSNS];

#endif
