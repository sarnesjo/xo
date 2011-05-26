#ifndef XO_FLAG_SET_H
#define XO_FLAG_SET_H

#include <stdio.h>
#include "types.h"

#define XO_FLAG_SET_CF (1 << 0)
#define XO_FLAG_SET_OF (1 << 1)
#define XO_FLAG_SET_PF (1 << 2)
#define XO_FLAG_SET_SF (1 << 3)
#define XO_FLAG_SET_ZF (1 << 4)

void xo_flag_set_print(FILE *, xo_flag_set, const char *);

#endif
