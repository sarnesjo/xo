#ifndef XO_NODE_H
#define XO_NODE_H

#include "types.h"

void xo_node_init(xo_node *, size_t, size_t);
size_t xo_node_arity(const xo_node *n);
void xo_node_print(const xo_node *, const char *);

#endif
