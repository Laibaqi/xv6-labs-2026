#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

int
main(void)
{
  char *buf = malloc(16 * PGSIZE);
  unsigned int bits = 0;

  // clear any dirty bits left over from setup
  pgdirty(buf, 16, &bits);

  buf[PGSIZE * 1] = 1;
  buf[PGSIZE * 3] = 1;
  buf[PGSIZE * 5] = 1;

  bits = 0;
  if(pgdirty(buf, 16, &bits) < 0){
    printf("dirtytest: pgdirty failed\n");
    exit(1);
  }
  if(bits != ((1 << 1) | (1 << 3) | (1 << 5))){
    printf("dirtytest: wrong bits 0x%x\n", bits);
    exit(1);
  }

  // a second call must report nothing, since the bits were cleared
  bits = 0;
  pgdirty(buf, 16, &bits);
  if(bits != 0){
    printf("dirtytest: bits not cleared 0x%x\n", bits);
    exit(1);
  }
  printf("dirtytest: OK\n");
  exit(0);
}
