#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "insns.h"
#include "instruction.h"
#include "invocation.h"
#include "parser.h"
#include "program.h"
#include "xo.h"

typedef enum
{
  XO_ACTION_GENERATE_PROGRAM,
  XO_ACTION_LIST_INSNS,
  XO_ACTION_SHOW_HELP,
  XO_ACTION_SHOW_VERSION,
} xo_action;

xo_action action = XO_ACTION_GENERATE_PROGRAM;
int VERBOSITY;
char *GOAL_PROGRAM_STR;

void did_parse_insn(size_t i, const xo_instruction *insn, size_t r0, size_t r1, void *userdata)
{
  xo_program *goal_program = userdata;
  xo_invocation_init(&goal_program->invocations[i], insn, r0, r1);
}

void did_generate_program(const xo_program *program, void *userdata)
{
  // TODO: test program for equivalence with goal program
  xo_program_print(program, "\n");
}

void generate_program()
{
  size_t num_insns_in_goal_program = xo_parser_count_insns(GOAL_PROGRAM_STR);
  if(num_insns_in_goal_program == 0)
  {
    fprintf(stderr, "invalid goal program\n");
    exit(EXIT_FAILURE);
  }

  xo_program *goal_program = xo_program_create(num_insns_in_goal_program);
  xo_parser_traverse(GOAL_PROGRAM_STR, did_parse_insn, goal_program);
  xo_program_print(goal_program, "\n");

  for(size_t num_insns = 1; num_insns <= num_insns_in_goal_program; ++num_insns) // TODO: consider measures of optimality besides insn count
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
  printf("\t%s [-qv] -g GOAL_PROGRAM\n", PACKAGE_NAME);
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
  int o;
  while((o = getopt(argc, argv, "LHVqvg:")) != -1)
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
        --VERBOSITY;
        break;
      case 'v':
        ++VERBOSITY;
        break;
      case 'g':
        free(GOAL_PROGRAM_STR);
        GOAL_PROGRAM_STR = strdup(optarg);
        break;
      default:
        exit(1);
    }
  }
  argc -= optind;
  argv += optind;

  if(action == XO_ACTION_GENERATE_PROGRAM && !GOAL_PROGRAM_STR)
    action = XO_ACTION_SHOW_HELP;

  switch(action)
  {
    case XO_ACTION_GENERATE_PROGRAM:
      generate_program();
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
