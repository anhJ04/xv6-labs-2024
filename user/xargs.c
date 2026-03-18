#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
  char buf[512];
  char *new_argv[MAXARG];
  int i;

  if (argc < 2) {
    fprintf(2, "Usage: xargs command [args...]\n");
    exit(1);
  }
  for (i = 1; i < argc; i++) {
    new_argv[i - 1] = argv[i];
  }

  while (1) {
    int count = 0;
    int n;
    while ((n = read(0, &buf[count], 1)) > 0) {
      if (buf[count] == '\n') {
        break;
      }
      count++;
    }

    if (n <= 0) break;

    buf[count] = '\0'; 
    new_argv[argc - 1] = buf;
    new_argv[argc] = 0;   

    if (fork() == 0) {
      exec(new_argv[0], new_argv);
      exit(0);
    } else {
      wait(0); 
    }
  }

  exit(0);
}