#ifndef XO_REGISTER_SET_H
#define XO_REGISTER_SET_H

#include "types.h"

#define XO_REGISTER_SET_R0 (1 << 0)
#define XO_REGISTER_SET_R1 (1 << 1)
#define XO_REGISTER_SET_R2 (1 << 2)
#define XO_REGISTER_SET_R3 (1 << 3)
#define XO_REGISTER_SET_R4 (1 << 4)
#define XO_REGISTER_SET_R5 (1 << 5)
#define XO_REGISTER_SET_R6 (1 << 6)
#define XO_REGISTER_SET_R7 (1 << 7)

xo_register_set xo_register_set_from_index(size_t);
size_t xo_register_set_first_live_index(xo_register_set);
size_t xo_register_set_first_dead_index(xo_register_set);

#endif
