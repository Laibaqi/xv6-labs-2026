#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "user/user.h"

int matchhere(char *, char *);
int matchstar(int, char *, char *);

int
match_regexp(char *re, char *text)
{
  if (re[0] == '^')
    return matchhere(re + 1, text);
  do {
    if (matchhere(re, text))
      return 1;
  } while (*text++ != '\0');
  return 0;
}

int
matchhere(char *re, char *text)
{
  if (re[0] == '\0')
    return 1;
  if (re[1] == '*')
    return matchstar(re[0], re + 2, text);
  if (re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if (*text != '\0' && (re[0] == '.' || re[0] == *text))
    return matchhere(re + 1, text + 1);
  return 0;
}

int
matchstar(int c, char *re, char *text)
{
  do {
    if (matchhere(re, text))
      return 1;
  } while (*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  if(strlen(p) >= DIRSIZ) return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void
run_exec(char *file, char **execargv, int execargc)
{
  char *args[MAXARG];
  int i;
  for(i = 0; i < execargc; i++) args[i] = execargv[i];
  args[i++] = file;
  args[i] = 0;

  int pid = fork();
  if(pid == 0){
    exec(args[0], args);
    fprintf(2, "find: exec %s failed\n", args[0]);
    exit(1);
  } else {
    wait(0);
  }
}

void
match(char *path, char **execargv, int execargc)
{
  if(execargc > 0)
    run_exec(path, execargv, execargc);
  else
    printf("%s\n", path);
}

void
find(char *path, char *name, char **execargv, int execargc)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(match_regexp(name, fmtname(path)) || strcmp(fmtname(path), name) == 0)
      match(path, execargv, execargc);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0) continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      if(match_regexp(name, fmtname(buf)) || strcmp(fmtname(buf), name) == 0)
        match(buf, execargv, execargc);
      if(st.type == T_DIR)
        find(buf, name, execargv, execargc);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  char *execargv[MAXARG];
  int execargc = 0;

  if(argc < 3){
    fprintf(2, "Usage: find path name [-exec cmd ...]\n");
    exit(1);
  }

  for(int i = 3; i < argc; i++){
    if(strcmp(argv[i], "-exec") == 0){
      for(i = i+1; i < argc && execargc < MAXARG-2; i++)
        execargv[execargc++] = argv[i];
      break;
    }
  }

  find(argv[1], argv[2], execargv, execargc);
  exit(0);
}
