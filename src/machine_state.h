#ifndef XO_MACHINE_STATE_H
#define XO_MACHINE_STATE_H

#include <stdio.h>
#include "types.h"

void xo_machine_state_init(xo_machine_state *, xo_register, xo_flag_set);
void xo_machine_state_copy(xo_machine_state *, const xo_machine_state *);
void xo_machine_state_print(FILE *, const xo_machine_state *, const char *);

xo_register xo_machine_state_get_cf(const xo_machine_state *);
xo_register xo_machine_state_get_of(const xo_machine_state *);
xo_register xo_machine_state_get_pf(const xo_machine_state *);
xo_register xo_machine_state_get_sf(const xo_machine_state *);
xo_register xo_machine_state_get_zf(const xo_machine_state *);

void xo_machine_state_set_cf(xo_machine_state *, xo_register);
void xo_machine_state_set_of(xo_machine_state *, xo_register);
void xo_machine_state_set_pf(xo_machine_state *, xo_register);
void xo_machine_state_set_sf(xo_machine_state *, xo_register);
void xo_machine_state_set_zf(xo_machine_state *, xo_register);

#endif
