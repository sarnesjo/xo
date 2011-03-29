#ifndef XO_GRAPH_H
#define XO_GRAPH_H

#include "types.h"

xo_graph *xo_graph_create(size_t);
void xo_graph_destroy(xo_graph *);
void xo_graph_print(const xo_graph *);

xo_supergraph *xo_supergraph_create(size_t, size_t);
void xo_supergraph_destroy(xo_supergraph *);
void xo_supergraph_print(const xo_supergraph *);

typedef void (*xo_graph_callback)(const xo_graph *);
void xo_graph_generate_from_supergraph(xo_graph *, const xo_supergraph *, xo_graph_callback);

#endif
