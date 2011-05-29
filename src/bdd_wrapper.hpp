#ifndef XO_BDD_WRAPPER_HPP
#define XO_BDD_WRAPPER_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include "instruction.h"
#include "types.h"

void xo_bdd_evaluate_program_on_state(const xo_program *, xo_machine_state *);

#ifdef __cplusplus
}
#endif

#endif
