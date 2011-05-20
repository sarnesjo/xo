#ifndef XO_PARSER_H
#define XO_PARSER_H

#include <stdbool.h>
#include "types.h"

bool xo_parser_validate(const char *);
size_t xo_parser_count_insns(const char *);

typedef void (*xo_parser_traverse_callback)(size_t, const xo_instruction *, size_t, size_t, void *);
void xo_parser_traverse(const char *, xo_parser_traverse_callback, void *userdata);

#endif
