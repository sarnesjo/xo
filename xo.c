#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "graph.h"
#include "insns.h"
#include "xo.h"

typedef enum
{
  XO_ACTION_GENERATE_GRAPH,
  XO_ACTION_LIST_INSNS,
  XO_ACTION_SHOW_HELP,
  XO_ACTION_SHOW_VERSION,
} xo_action;

xo_action action = XO_ACTION_GENERATE_GRAPH;
int verbosity;
int num_inputs, num_insns;

void graph_callback(const xo_graph *graph)
{
  xo_graph_print(graph);
}

void generate_graph()
{
  xo_graph *graph = xo_graph_create(num_inputs+num_insns);
  xo_supergraph *supergraph = xo_supergraph_create(num_inputs+num_insns, num_inputs);
  xo_graph_generate_from_supergraph(graph, supergraph, graph_callback);
}

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
  printf("\t%s [-qv] -i NUM_INPUTS -n NUM_INSNS\n", PACKAGE_NAME);
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
  while((o = getopt(argc, argv, "LHVqvi:n:")) != -1)
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
      case 'i':
        num_inputs = strtol(optarg, NULL, 0);
        break;
      case 'n':
        num_insns = strtol(optarg, NULL, 0);
        break;
      default:
        exit(1);
    }
  }
  argc -= optind;
  argv += optind;

  if(action == XO_ACTION_GENERATE_GRAPH && !(num_inputs > 0 && num_insns > 0))
    action = XO_ACTION_SHOW_HELP;

  switch(action)
  {
    case XO_ACTION_GENERATE_GRAPH:
      generate_graph();
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
