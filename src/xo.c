#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bdd_wrapper.hpp"
#include "generator.h"
#include "insns.h"
#include "instruction.h"
#include "program.h"
#include "register_set.h"
#include "test_states.h"

typedef enum
{
  XO_ACTION_GENERATE_PROGRAM,
  XO_ACTION_LIST_INSNS,
  XO_ACTION_SHOW_HELP,
  XO_ACTION_SHOW_VERSION,
} xo_action;

void did_generate_program(const xo_program *program, xo_register_set input_regs, xo_register_set output_regs, void *userdata)
{
  xo_program *goal_program = userdata;

  if(xo_program_equivalent_on_states(goal_program, program, XO_NUM_TEST_STATES, xo_test_states))
  {
    xo_program_print(stdout, program, "?\n");

    if(xo_bdd_equivalent_programs(goal_program, program))
      xo_program_print(stdout, program, "!\n");
  }
}

void generate_program(const char *goal_program_str)
{
  xo_program *goal_program = xo_program_create_from_str(goal_program_str);
  if(!goal_program)
  {
    fprintf(stderr, "invalid goal program\n");
    exit(EXIT_FAILURE);
  }

  xo_register_set input_regs = 0, output_regs = 0;
  xo_program_analyze(goal_program, &input_regs, &output_regs);

  fprintf(stderr, "goal program: ");
  xo_program_print(stderr, goal_program, "\n");

  fprintf(stderr, "input registers:");
  for(size_t i = 0; i < XO_NUM_REGISTERS; ++i)
    if(input_regs & (1 << i))
      fprintf(stderr, " r%zu", i);
  fprintf(stderr, "\n");

  fprintf(stderr, "output register:");
  for(size_t o = 0; o < XO_NUM_REGISTERS; ++o)
    if(output_regs & (1 << o))
      fprintf(stderr, " r%zu", o);
  fprintf(stderr, "\n");

  // TODO: consider measures of optimality besides insn count
  for(size_t num_invocations = 1; num_invocations <= goal_program->num_invocations; ++num_invocations)
  {
    fprintf(stderr, "%zu...\n", num_invocations);
    xo_generator_generate_programs(num_invocations, input_regs, output_regs, did_generate_program, goal_program);
  }

  xo_program_destroy(goal_program);
}

void list_insns()
{
  for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    xo_instruction_print(stdout, &xo_insns[i], "\n");
}

void show_help()
{
  printf("usage:\n");
  printf("\t%s [-qv] GOAL_PROGRAM\n", PACKAGE_NAME);
  printf("\t%s -L\n", PACKAGE_NAME);
  printf("\t%s -H\n", PACKAGE_NAME);
  printf("\t%s -V\n", PACKAGE_NAME);
}

void show_version()
{
  printf("%s\n", PACKAGE_STRING);
}

int main(int argc, char *argv[])
{
  xo_action action = XO_ACTION_GENERATE_PROGRAM;
  int verbosity;

  int o;
  while((o = getopt(argc, argv, "LHVqv")) != -1)
  {
    switch(o)
    {
      case 'L':
        action = XO_ACTION_LIST_INSNS;
        break;
      case 'H':
        action = XO_ACTION_SHOW_HELP;
        break;
      case 'V':
        action = XO_ACTION_SHOW_VERSION;
        break;
      case 'q':
        --verbosity;
        break;
      case 'v':
        ++verbosity;
        break;
      default:
        exit(1);
    }
  }
  argc -= optind;
  argv += optind;

  if(action == XO_ACTION_GENERATE_PROGRAM && argc != 1)
    action = XO_ACTION_SHOW_HELP;

  switch(action)
  {
    case XO_ACTION_GENERATE_PROGRAM:
      generate_program(argv[0]);
      break;
    case XO_ACTION_LIST_INSNS:
      list_insns();
      break;
    case XO_ACTION_SHOW_HELP:
      show_help();
      break;
    case XO_ACTION_SHOW_VERSION:
      show_version();
      break;
  }

  return 0;
}
