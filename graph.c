#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "combinatorics.h"
#include "graph.h"
#include "node.h"

xo_graph *xo_graph_create(size_t num_nodes)
{
  xo_graph *graph = malloc(sizeof(xo_graph));
  if(graph)
  {
    graph->num_nodes = num_nodes;
    graph->nodes = malloc(num_nodes * sizeof(xo_node)); // TODO: check this malloc
  }
  return graph;
}

void xo_graph_destroy(xo_graph *graph)
{
  free(graph->nodes);
  free(graph);
}

void xo_graph_print(const xo_graph *graph)
{
  for(size_t n = 0; n < graph->num_nodes; ++n)
  {
    printf("%zu:", n);
    xo_node_print(&graph->nodes[n], " ");
  }
  printf("\n");
}

// --

xo_supergraph *xo_supergraph_create(size_t num_nodes, size_t num_skipped_nodes)
{
  xo_supergraph *supergraph = malloc(sizeof(xo_supergraph)); // TODO: check this malloc
  supergraph->num_nodes = num_nodes;
  supergraph->nodes = malloc(num_nodes * sizeof(xo_graph)); // TODO: check this malloc

  for(size_t n = 0; n < num_skipped_nodes; ++n)
  {
    supergraph->nodes[n].num_nodes = 0;
    supergraph->nodes[n].nodes = NULL;
  }

  for(size_t n = num_skipped_nodes; n < num_nodes; ++n)
  {
    // node n (= 0, 1, 2...) has...
    // n multichoose 0 = 1 (nullary)
    // n multichoose 1 = n (unary)
    // n multichoose 2 = n+1 choose 2 (binary)
    // ... possible dependency configurations
    size_t num = 1 + n + xo_choose(n+1, 2);
    supergraph->nodes[n].num_nodes = num;
    supergraph->nodes[n].nodes = malloc(num * sizeof(xo_node)); // TODO: check this malloc

    size_t v = 0;
    xo_node_init(&supergraph->nodes[n].nodes[v++], XO_NODE_DEPENDENCY_NONE, XO_NODE_DEPENDENCY_NONE);
    for(size_t d0 = 0; d0 < n; ++d0)
      xo_node_init(&supergraph->nodes[n].nodes[v++], d0, XO_NODE_DEPENDENCY_NONE);
    for(size_t d0 = 0; d0 < n; ++d0)
      for(size_t d1 = d0; d1 < n; ++d1)
        xo_node_init(&supergraph->nodes[n].nodes[v++], d0, d1);
  }

  return supergraph;
}

void xo_supergraph_destroy(xo_supergraph *supergraph)
{
  // TODO
}

void xo_supergraph_print(const xo_supergraph *supergraph)
{
  for(size_t n = 0; n < supergraph->num_nodes; ++n)
  {
    printf("%zu:", n);
    for(size_t v = 0; v < supergraph->nodes[n].num_nodes; ++v)
      xo_node_print(&supergraph->nodes[n].nodes[v], " ");
    printf("\n");
  }
}

// --

void xo_graph_generate_from_supergraph_(xo_graph *graph, const xo_supergraph *supergraph, xo_graph_callback callback, size_t n)
{
  assert(graph->num_nodes == supergraph->num_nodes); // TODO: let this function create graph internally instead?

  if(supergraph->nodes[n].num_nodes > 0)
  {
    for(size_t v = 0; v < supergraph->nodes[n].num_nodes; ++v)
    {
      graph->nodes[n] = supergraph->nodes[n].nodes[v];
      if(n == graph->num_nodes-1)
        callback(graph);
      else
        xo_graph_generate_from_supergraph_(graph, supergraph, callback, n+1);
    }
  }
  else
  {
    // TODO: node should be marked as being fixed, not just as having no dependencies
    xo_node_init(&graph->nodes[n], XO_NODE_DEPENDENCY_NONE, XO_NODE_DEPENDENCY_NONE);
    xo_graph_generate_from_supergraph_(graph, supergraph, callback, n+1);
  }
}

void xo_graph_generate_from_supergraph(xo_graph *graph, const xo_supergraph *supergraph, xo_graph_callback callback)
{
  xo_graph_generate_from_supergraph_(graph, supergraph, callback, 0);
}
