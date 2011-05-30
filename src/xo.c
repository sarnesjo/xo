#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "equivalence_checker_bdd.h"
#include "equivalence_checker_c.h"
#include "generator.h"
#include "insns.h"
#include "instruction.h"
#include "program.h"
#include "register_set.h"
#include "test_states.h"

typedef enum
{
  XO_ACTION_GENERATE_PROGRAMS,
  XO_ACTION_LIST_INSNS,
  XO_ACTION_SHOW_HELP,
  XO_ACTION_SHOW_VERSION,
} xo_action;

static int verbosity_ = 0;

static void did_generate_program_(const xo_program *program, void *userdata)
{
  xo_program *input_program = userdata;

  if(verbosity_ >= 2)
    xo_program_print(stderr, program, "?\n");

  if(xo_equivalence_checker_c_programs_equivalent_on_states(input_program, program, XO_NUM_TEST_STATES, xo_test_states))
  {
    if(verbosity_ >= 1)
      xo_program_print(stderr, program, "!\n");

    if(xo_equivalence_checker_bdd_programs_equivalent(input_program, program))
      xo_program_print(stdout, program, "!!\n");
  }
}

static void generate_programs_(const char *input_program_str, size_t max_num_invocations)
{
  xo_program *input_program = xo_program_create_from_str(input_program_str);
  if(!input_program)
  {
    fprintf(stderr, "invalid input program\n");
    exit(EXIT_FAILURE);
  }

  xo_register_set input_regs = 0, output_regs = 0;
  xo_program_analyze(input_program, &input_regs, &output_regs);

  if(verbosity_ >= 1)
  {
    fprintf(stderr, "input program: ");
    xo_program_print(stderr, input_program, "\n");

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
  }

  for(size_t num_invocations = 1; num_invocations <= max_num_invocations; ++num_invocations)
  {
    if(verbosity_ >= 1)
      fprintf(stderr, "%zu...\n", num_invocations);

    xo_generator_generate_programs(num_invocations, input_regs, output_regs, did_generate_program_, input_program);
  }

  xo_program_destroy(input_program);
}

static void list_insns_()
{
  for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    xo_instruction_print(stdout, &xo_insns[i], "\n");
}

static void show_help_()
{
  printf("usage:\n");
  printf("\t%s [-m M] [-qv] PROGRAM\n", PACKAGE_NAME);
  printf("\t%s -L\n", PACKAGE_NAME);
  printf("\t%s -H\n", PACKAGE_NAME);
  printf("\t%s -V\n", PACKAGE_NAME);
}

static void show_version_()
{
  printf("%s\n", PACKAGE_STRING);
}

int main(int argc, char *argv[])
{
  xo_action action = XO_ACTION_GENERATE_PROGRAMS;
  size_t max_num_invocations = 5;

  int o;
  while((o = getopt(argc, argv, "LHVm:qv")) != -1)
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
      case 'm':
        max_num_invocations = strtol(optarg, NULL, 10);
        break;
      case 'q':
        --verbosity_;
        break;
      case 'v':
        ++verbosity_;
        break;
      default:
        exit(1);
    }
  }
  argc -= optind;
  argv += optind;

  if(action == XO_ACTION_GENERATE_PROGRAMS && argc != 1)
    action = XO_ACTION_SHOW_HELP;

  switch(action)
  {
    case XO_ACTION_GENERATE_PROGRAMS:
      generate_programs_(argv[0], max_num_invocations);
      break;
    case XO_ACTION_LIST_INSNS:
      list_insns_();
      break;
    case XO_ACTION_SHOW_HELP:
      show_help_();
      break;
    case XO_ACTION_SHOW_VERSION:
      show_version_();
      break;
  }

  return 0;
}
