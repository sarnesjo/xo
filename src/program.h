#ifndef XO_PROGRAM_H
#define XO_PROGRAM_H

#include <stdio.h>
#include "types.h"

xo_program *xo_program_create(size_t);
xo_program *xo_program_create_from_str(const char *);
void xo_program_destroy(xo_program *);

void xo_program_analyze(const xo_program *, xo_register_set *, xo_register_set *);

bool xo_program_equal(const xo_program *, const xo_program *);

void xo_program_print(FILE *, const xo_program *, const char *);

#endif
