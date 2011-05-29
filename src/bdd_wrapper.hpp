#ifndef XO_BDD_WRAPPER_HPP
#define XO_BDD_WRAPPER_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include "instruction.h"
#include "program.h"
#include "register_set.h"
#include "types.h"

void xo_bdd_evaluate_program_on_state(const xo_program *, xo_machine_state *);
bool xo_bdd_equivalent_programs(const xo_program *, const xo_program *);

#ifdef __cplusplus
}
#endif

#endif
