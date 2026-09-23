#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  int n = 100 * 4096;
  char *p = sbrk(n);
  if (p != (char *)-1) {
    char *end = p + n;
    for (char *s = p; s <= end - 12; s++) {
      if (memcmp(s, "Here it is: ", 12) == 0) {
        char *secret = s + 12;
        printf("%s\n", secret);
        exit(0);
      }
    }
  }

  exit(1);
}
