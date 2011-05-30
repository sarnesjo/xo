#ifndef XO_EQUIVALENCE_CHECKER_BDD_H
#define XO_EQUIVALENCE_CHECKER_BDD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "instruction.h"
#include "program.h"
#include "register_set.h"
#include "types.h"

void xo_equivalence_checker_bdd_evaluate_program_on_state(const xo_program *, xo_machine_state *);
bool xo_equivalence_checker_bdd_programs_equivalent(const xo_program *, const xo_program *);

#ifdef __cplusplus
}
#endif

#endif
