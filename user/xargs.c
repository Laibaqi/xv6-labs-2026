#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: xargs command [args...]\n");
    exit(1);
  }

  char *args[MAXARG];
  int base_count = 0;

  for(int i = 1; i < argc; i++){
    args[base_count++] = argv[i];
  }

  char line[512];
  int n = 0;
  char buf;

  while(read(0, &buf, 1) > 0){
    if(buf == '\n'){
      line[n] = 0;
      char *exec_args[MAXARG];
      int count = 0;

      for(int j = 0; j < base_count; j++){
        exec_args[count++] = args[j];
      }

      char *p = line;
      while(*p){
        while(*p == ' ' || *p == '\t') {
          *p = 0;
          p++;
        }
        if(*p == 0) break;
        if(count < MAXARG - 1){
          exec_args[count++] = p;
        }
        while(*p && *p != ' ' && *p != '\t') p++;
      }
      exec_args[count] = 0;

      if(count > 0){
        int pid = fork();
        if(pid == 0){
          exec(exec_args[0], exec_args);
          fprintf(2, "xargs: exec %s failed\n", exec_args[0]);
          exit(1);
        }
        wait(0);
      }
      n = 0;
    } else {
      if(n < sizeof(line) - 1){
        line[n++] = buf;
      }
    }
  }

  exit(0);
}
