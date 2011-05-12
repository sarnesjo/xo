#ifndef XO_TESTS_CHECK_H
#define XO_TESTS_CHECK_H

#include <stdio.h>
#include <stdlib.h>

#define CHECK(x) do { if(!(x)) { fprintf(stderr, "FAILED: %s, %s:%d\n", #x, __FILE__, __LINE__); exit(EXIT_FAILURE); } } while(0)

#endif
