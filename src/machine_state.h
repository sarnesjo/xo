#ifndef XO_MACHINE_STATE_H
#define XO_MACHINE_STATE_H

#include <stdio.h>
#include "types.h"

void xo_machine_state_init(xo_machine_state *, uint32_t, xo_flag_set);
void xo_machine_state_copy(xo_machine_state *, const xo_machine_state *);
void xo_machine_state_print(FILE *, const xo_machine_state *, const char *);

#endif
