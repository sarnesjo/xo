#ifndef XO_TYPES_H
#define XO_TYPES_H

#include "std.h"

// these values should not be changed
#define XO_NUM_BITS 32
#define XO_NUM_FLAGS 5

#define XO_NUM_REGISTERS 8
#define XO_REGISTER_NONE ((size_t)-1)

typedef uint32_t xo_register;    // should be defined as uintN_t, where N == XO_NUM_BITS
typedef uint8_t xo_register_set; // should be defined as uintN_t, where N == XO_NUM_REGISTERS
typedef uint8_t xo_flag_set;     // should be defined as uintN_t, where N >= XO_NUM_FLAGS

// machine state

typedef struct
{
  xo_register regs[XO_NUM_REGISTERS];
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
