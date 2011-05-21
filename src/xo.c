#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "insns.h"
#include "instruction.h"
#include "program.h"

typedef enum
{
  XO_ACTION_GENERATE_PROGRAM,
  XO_ACTION_LIST_INSNS,
  XO_ACTION_SHOW_HELP,
  XO_ACTION_SHOW_VERSION,
} xo_action;

void did_generate_program(const xo_program *program, void *userdata)
{
  // TODO: test program for equivalence with goal program
  xo_program_print(program, "\n");
}

void generate_program(const char *goal_program_str)
{
  xo_program *goal_program = xo_program_create_from_str(goal_program_str);
  if(!goal_program)
  {
    fprintf(stderr, "invalid goal program\n");
    exit(EXIT_FAILURE);
  }

  xo_program_print(goal_program, "\n");

  for(size_t num_insns = 1; num_insns <= goal_program->num_invocations; ++num_insns) // TODO: consider measures of optimality besides insn count
  {
    fprintf(stderr, "%zu...\n", num_insns);

    xo_program *program = xo_program_create(num_insns);
    xo_program_generate(program, did_generate_program, NULL);
    xo_program_destroy(program);
  }

  xo_program_destroy(goal_program);
}

void list_insns()
{
  for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    xo_instruction_print(&xo_insns[i], "\n");
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
