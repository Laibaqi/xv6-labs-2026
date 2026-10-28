//
// Look up kernel code addresses in the symbol tables.
//

#include "types.h"
#include "ksym.h"

// Return the index of the last entry whose addr <= pc, or -1.
// Entries are `stride` bytes apart; addr is the first field.
static int
search(const char *base, int n, int stride, uint64 pc)
{
  int lo = 0, hi = n - 1, ans = -1;
  while (lo <= hi) {
    int mid = (lo + hi) / 2;
    uint64 a = *(const uint64 *)(base + (uint64)mid * stride);
    if (a <= pc) {
      ans = mid;
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }
  return ans;
}

// Find the function and source line for code address pc.
// Returns 0 if pc is not in any known function.
int
ksym_lookup(uint64 pc, const char **name, uint64 *off,
            const char **file, int *line)
{
  extern char etext[]; // end of kernel code, from kernel.ld

  if (ksym_nfuncs == 0 || pc < ksym_funcs[0].addr || pc >= (uint64)etext)
    return 0;

  int f = search((const char *)&ksym_funcs[0].addr, ksym_nfuncs,
                 sizeof(struct ksym_func), pc);
  if (f < 0)
    return 0;
  *name = ksym_funcs[f].name;
  *off = pc - ksym_funcs[f].addr;

  *file = 0;
  *line = 0;
  int l = search((const char *)&ksym_lines[0].addr, ksym_nlines,
                 sizeof(struct ksym_line), pc);
  if (l >= 0) {
    *file = ksym_files[ksym_lines[l].file];
    *line = ksym_lines[l].line;
  }
  return 1;
}