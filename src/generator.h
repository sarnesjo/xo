#ifndef XO_GENERATOR_H
#define XO_GENERATOR_H

#include "types.h"

typedef void (*xo_generator_callback)(const xo_program *, void *);
void xo_generator_generate_programs(size_t num_invocations, xo_register_set, xo_register_set, xo_generator_callback, void *);

#endif
