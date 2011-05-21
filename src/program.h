#ifndef XO_PROGRAM_H
#define XO_PROGRAM_H

#include "types.h"

xo_program *xo_program_create(size_t);
xo_program *xo_program_create_from_str(const char *);
void xo_program_destroy(xo_program *);
void xo_program_run(const xo_program *, xo_machine_state *);
void xo_program_print(const xo_program *, const char *);
void xo_program_analyze(const xo_program *, uint8_t *, uint8_t *);

#endif
