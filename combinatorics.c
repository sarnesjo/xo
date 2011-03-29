#include "combinatorics.h"

int xo_factorial(int n)
{
  int f = 1;
  while(n > 1)
    f *= n--;
  return f;
}

int xo_choose(int n, int k)
{
  return xo_factorial(n) / (xo_factorial(k) * xo_factorial(n-k));
}

int xo_multichoose(int n, int k)
{
  return xo_choose(n+k-1, k);
}
