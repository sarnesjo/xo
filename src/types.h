#ifndef XO_TYPES_H
#define XO_TYPES_H

#include "std.h"

#define XO_REGISTER_NONE ((size_t)-1)
#define XO_NUM_REGISTERS 8

typedef uint8_t xo_register_set; // this should be defined as uintN_t, where N is XO_NUM_REGISTERS
typedef uint8_t xo_flag_set;

#pragma mark -
#pragma mark machine state

typedef struct
{
  uint32_t regs[XO_NUM_REGISTERS];
  uint32_t cf;
  uint32_t of;
  uint32_t pf;
  uint32_t sf;
  uint32_t zf;
} xo_machine_state;

#pragma mark -
#pragma mark instruction

typedef void (*xo_instruction_impl)(xo_machine_state *, size_t, size_t);

typedef struct
{
  char *name;
  size_t arity;
  xo_register_set input_regs;
  xo_register_set output_regs;
  xo_flag_set input_flags;
  xo_flag_set output_flags;
  xo_flag_set defined_flags;
  xo_instruction_impl impl;
} xo_instruction;

#pragma mark -
#pragma mark invocation

typedef struct
{
  const xo_instruction *insn;
  size_t r0;
  size_t r1;
} xo_invocation;

#pragma mark -
#pragma mark program

typedef struct
{
  size_t num_invocations;
  xo_invocation *invocations;
} xo_program;

#endif
