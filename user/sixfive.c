#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
is_sep(char c)
{
  char *sep = " -\r\t\n./,";
  for(char *p = sep; *p; p++)
    if(c == *p) return 1;
  return 0;
}

void
process(int fd)
{
  char c;
  int num = 0, in_num = 0, n;

  while((n = read(fd, &c, 1)) > 0){
    if(c >= '0' && c <= '9'){
      num = num * 10 + (c - '0');
      in_num = 1;
    } else if(is_sep(c)){
      if(in_num){
        if(num % 5 == 0 || num % 6 == 0)
          printf("%d\n", num);
      }
      num = 0;
      in_num = 0;
    }
  }
  if(in_num && (num % 5 == 0 || num % 6 == 0))
    printf("%d\n", num);
}

int
main(int argc, char *argv[])
{
  if(argc <= 1){
    process(0);
    exit(0);
  }
  for(int i = 1; i < argc; i++){
    int fd = open(argv[i], 0);
    if(fd < 0){
      printf("sixfive: cannot open %s\n", argv[i]);
      continue;
    }
    process(fd);
    close(fd);
  }
  exit(0);
}
