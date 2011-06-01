#ifndef XO_FLAG_SET_H
#define XO_FLAG_SET_H

#include <stdio.h>
#include "types.h"

enum
{
  XO_FLAG_OFFSET_CF = 0,
  XO_FLAG_OFFSET_OF = 1,
  XO_FLAG_OFFSET_PF = 2,
  XO_FLAG_OFFSET_SF = 3,
  XO_FLAG_OFFSET_ZF = 4,
};

enum
{
  XO_FLAG_SET_CF = (1 << XO_FLAG_OFFSET_CF),
  XO_FLAG_SET_OF = (1 << XO_FLAG_OFFSET_OF),
  XO_FLAG_SET_PF = (1 << XO_FLAG_OFFSET_PF),
  XO_FLAG_SET_SF = (1 << XO_FLAG_OFFSET_SF),
  XO_FLAG_SET_ZF = (1 << XO_FLAG_OFFSET_ZF),
};

void xo_flag_set_print(FILE *, xo_flag_set, const char *);

#endif
