#ifndef XO_TYPES_H
#define XO_TYPES_H

#include "std.h"

#define XO_REGISTER_NONE ((size_t)-1)
#define XO_NUM_REGISTERS 8

typedef uint8_t xo_register_set; // this should be defined as uintN_t, where N is XO_NUM_REGISTERS
typedef uint8_t xo_flag_set;

// machine state

typedef struct
{
  uint32_t regs[XO_NUM_REGISTERS];
  xo_flag_set flags;
} xo_machine_state;

// instruction

typedef struct
{
  const int id;
  const char *name;
  size_t arity;
  xo_register_set input_regs;
  xo_register_set output_regs;
  xo_flag_set input_flags;
  xo_flag_set output_flags;
  xo_flag_set live_output_flags;
} xo_instruction;

// invocation

typedef struct
{
  const xo_instruction *insn;
  size_t r0;
  size_t r1;
} xo_invocation;

// program

typedef struct
{
  size_t num_invocations;
  xo_invocation *invocations;
} xo_program;

#endif
