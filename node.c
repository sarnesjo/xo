#include <stdio.h>
#include "node.h"

void xo_node_init(xo_node *n, size_t dependency0, size_t dependency1)
{
  n->dependency0 = dependency0;
  n->dependency1 = dependency1;
}

void xo_node_print(const xo_node *n, const char *suffix)
{
  if(n->dependency0 != XO_NODE_DEPENDENCY_NONE && n->dependency1 != XO_NODE_DEPENDENCY_NONE)
    printf("%zu,%zu%s", n->dependency0, n->dependency1, suffix);
  else if(n->dependency0 != XO_NODE_DEPENDENCY_NONE)
    printf("%zu%s", n->dependency0, suffix);
  else
    printf("-%s", suffix);
}
