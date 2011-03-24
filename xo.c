#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum
{
  XO_ACTION_SHOW_HELP,
  XO_ACTION_SHOW_VERSION,
} xo_action;

xo_action action = XO_ACTION_SHOW_HELP;
int verbosity;

void show_help()
{
  printf("usage:\n");
  printf("\t%s [-qv]\n", PACKAGE_NAME);
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
  while((o = getopt(argc, argv, "HVqv")) != -1)
  {
    switch(o)
    {
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
    case XO_ACTION_SHOW_HELP:
      show_help();
      break;
    case XO_ACTION_SHOW_VERSION:
      show_version();
      break;
  }

  return 0;
}
