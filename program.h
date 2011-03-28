#ifndef XO_PROGRAM_H
#define XO_PROGRAM_H

#include "types.h"

xo_program *xo_program_create(size_t);
void xo_program_destroy(xo_program *);
void xo_program_run(const xo_program *, xo_machine_state *);

#endif
