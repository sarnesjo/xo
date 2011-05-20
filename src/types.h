#ifndef XO_TYPES_H
#define XO_TYPES_H

#include <stddef.h>
#include <stdint.h>

#define XO_REGISTER_NONE ((size_t)-1)

#pragma mark -
#pragma mark machine state

#define XO_MACHINE_STATE_NUM_REGS 8

typedef struct
{
  uint32_t regs[XO_MACHINE_STATE_NUM_REGS];
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
