// Empty symbol tables. Linked into the kernel by default, and as
// the first pass of a `make KSYMS=1` build.
#include "types.h"
#include "ksym.h"

const int ksym_nfuncs = 0;
const struct ksym_func ksym_funcs[] = {
  {0, 0},
};

const char *const ksym_files[] = {
  0,
};

const int ksym_nlines = 0;
const struct ksym_line ksym_lines[] = {
  {0, 0, 0},
};