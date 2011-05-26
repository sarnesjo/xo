#include "register_set.h"

xo_register_set xo_register_set_from_index(size_t i)
{
  return (1 << i);
}

// TODO: there are faster ways of doing this
size_t xo_register_set_first_live_index(xo_register_set s)
{
  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    if((s & xo_register_set_from_index(i)) != 0)
      return i;
  return XO_REGISTER_NONE;
}

// TODO: there are faster ways of doing this
size_t xo_register_set_first_dead_index(xo_register_set s)
{
  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    if((s & xo_register_set_from_index(i)) == 0)
      return i;
  return XO_REGISTER_NONE;
}

