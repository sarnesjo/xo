#ifndef XO_INSTRUCTION_H
#define XO_INSTRUCTION_H

#include "types.h"

xo_instruction *xo_instruction_get(const char *);
void xo_instruction_print(const xo_instruction *, const char *);

#endif
