#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  volatile int *p = 0;
  printf("nulltest: storing through a null pointer, expect a fault...\n");
  *p = 1;
  printf("nulltest: FAILED, no fault\n");
  exit(1);
}
