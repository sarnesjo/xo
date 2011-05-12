#ifndef XO_INVOCATION_H
#define XO_INVOCATION_H

#include "types.h"

void xo_invocation_init(xo_invocation *, const char *, size_t, size_t);
void xo_invocation_invoke(const xo_invocation *, xo_machine_state *);
void xo_invocation_print(const xo_invocation *, const char *);

#endif
