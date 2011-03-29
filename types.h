#ifndef XO_TYPES_H
#define XO_TYPES_H

#include <stddef.h>
#include <stdint.h>

#pragma mark -
#pragma mark node

#define XO_NODE_DEPENDENCY_NONE ((size_t)-1)

typedef struct
{
  size_t dependency0;
  size_t dependency1;
} xo_node;

#pragma mark -
#pragma mark graph

typedef struct
{
  size_t num_nodes;
  xo_node *nodes;
} xo_graph;

typedef xo_graph xo_node_variants;

typedef struct
{
  size_t num_nodes;
  xo_node_variants *nodes;
} xo_supergraph;

#pragma mark -
#pragma mark machine state

#define XO_MACHINE_STATE_NUM_REGS 4

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
  xo_instruction_impl impl;
} xo_instruction;

#pragma mark -
#pragma mark invocation

typedef struct
{
  xo_instruction *insn;
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
