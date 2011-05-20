#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "insns.h"
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

void parser_traverse_callback(size_t i, const char *insn_name, size_t r0, size_t r1, void *userdata)
{
  xo_program *goal_program = userdata;
  xo_invocation_init(&goal_program->invocations[i], insn_name, r0, r1);
}

void generate_program()
{
  size_t n = xo_parser_count_insns(GOAL_PROGRAM_STR);
  if(n == 0)
  {
    fprintf(stderr, "invalid goal program\n");
    exit(EXIT_FAILURE);
  }

  xo_program *goal_program = xo_program_create(n); // TODO: destroy when done
  xo_parser_traverse(GOAL_PROGRAM_STR, parser_traverse_callback, goal_program);
  xo_program_print(goal_program, "");
}

void list_insns()
{
  for(size_t i = 0; i < XO_NUM_INSNS; ++i)
    printf("%s\n", xo_insns[i].name);
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
