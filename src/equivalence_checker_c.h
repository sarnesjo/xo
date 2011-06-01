#ifndef XO_EQUIVALENCE_CHECKER_C_H
#define XO_EQUIVALENCE_CHECKER_C_H

#include "types.h"

void xo_equivalence_checker_c_run_program_on_state(const xo_program *, xo_machine_state *);
bool xo_equivalence_checker_c_programs_equivalent_on_states(const xo_program *, const xo_program *, size_t, const xo_machine_state *);

#endif
