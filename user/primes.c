#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
sieve(int lpipe)
{
  int prime;
  if(read(lpipe, &prime, sizeof(int)) != sizeof(int)){
    close(lpipe);
    exit(0);
  }

  printf("prime %d\n", prime);

  int rpipe[2];
  if(pipe(rpipe) < 0){
    fprintf(2, "primes: pipe failed\n");
    close(lpipe);
    exit(1);
  }

  int pid = fork();
  if(pid < 0){
    fprintf(2, "primes: fork failed\n");
    close(lpipe);
    close(rpipe[0]);
    close(rpipe[1]);
    exit(1);
  }

  if(pid == 0){
    close(lpipe);
    close(rpipe[1]);
    sieve(rpipe[0]);
  } else {
    close(rpipe[0]);
    int num;
    while(read(lpipe, &num, sizeof(int)) == sizeof(int)){
      if(num % prime != 0){
        write(rpipe[1], &num, sizeof(int));
      }
    }
    close(lpipe);
    close(rpipe[1]);
    wait(0);
    exit(0);
  }
}

int
main(int argc, char *argv[])
{
  int p[2];
  if(pipe(p) < 0){
    fprintf(2, "primes: pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if(pid < 0){
    fprintf(2, "primes: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    close(p[1]);
    sieve(p[0]);
  } else {
    close(p[0]);
    for(int i = 2; i <= 35; i++){
      write(p[1], &i, sizeof(int));
    }
    close(p[1]);
    wait(0);
    exit(0);
  }

  return 0;
}
