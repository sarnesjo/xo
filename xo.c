#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "insns.h"
#include "xo.h"

typedef enum
{
  XO_ACTION_LIST_INSNS,
  XO_ACTION_SHOW_HELP,
  XO_ACTION_SHOW_VERSION,
} xo_action;

xo_action action = XO_ACTION_SHOW_HELP;
int verbosity;

void list_insns()
{
  for(size_t i = 0; i < XO_NUM_INSNS0; ++i)
    printf("%s\n", xo_insns0[i].name);
  for(size_t i = 0; i < XO_NUM_INSNS1; ++i)
    printf("%s\n", xo_insns1[i].name);
  for(size_t i = 0; i < XO_NUM_INSNS2; ++i)
    printf("%s\n", xo_insns2[i].name);
}

void show_help()
{
  printf("usage:\n");
  printf("\t%s [-qv]\n", PACKAGE_NAME);
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

  switch(action)
  {
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
