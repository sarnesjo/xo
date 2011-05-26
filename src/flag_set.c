#include "flag_set.h"

void xo_flag_set_print(FILE *file, xo_flag_set flags, const char *suffix)
{
  fprintf(file, "%s|%s|%s|%s|%s%s",
      (flags & XO_FLAG_SET_CF) ? "CF" : "",
      (flags & XO_FLAG_SET_OF) ? "OF" : "",
      (flags & XO_FLAG_SET_PF) ? "PF" : "",
      (flags & XO_FLAG_SET_SF) ? "SF" : "",
      (flags & XO_FLAG_SET_ZF) ? "ZF" : "",
      suffix);
}
