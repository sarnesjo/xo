#ifndef XO_INVOCATION_H
#define XO_INVOCATION_H

#include <stdio.h>
#include "types.h"

void xo_invocation_init(xo_invocation *, const xo_instruction *, size_t, size_t);
bool xo_invocation_equal(const xo_invocation *, const xo_invocation *);
void xo_invocation_invoke(const xo_invocation *, xo_machine_state *);
void xo_invocation_print(FILE *, const xo_invocation *, const char *);

#endif
