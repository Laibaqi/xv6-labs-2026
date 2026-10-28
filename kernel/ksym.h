// Kernel symbol table, used by backtrace() to print function
// names and source lines.

struct ksym_func {
  uint64 addr;      // first instruction of the function
  const char *name;
};

struct ksym_line {
  uint64 addr;      // first instruction for this source line
  uint32 line;
  uint32 file;      // index into ksym_files[]
};

extern const int ksym_nfuncs;
extern const struct ksym_func ksym_funcs[];
extern const int ksym_nlines;
extern const struct ksym_line ksym_lines[];
extern const char *const ksym_files[];