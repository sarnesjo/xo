#ifndef XO_PARSER_H
#define XO_PARSER_H

#include <stdbool.h>

bool xo_parser_validate(const char *);
size_t xo_parser_count_insns(const char *);

#endif
