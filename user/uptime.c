#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  uint ticks = uptime();
  printf("%d\n", ticks);
  exit(0);
}
