#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void fmt_name(char *path, int depth) {
  for (int i = 0; i < depth; i++) {
    printf("  "); 
  }
}

void tree(char *path, int depth) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "tree: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "tree: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type == T_FILE) {
    char *name = path;
    for(p=path+strlen(path); p >= path && *p != '/'; p--);
    if (*p == '/') name = p + 1;
    
    fmt_name(path, depth);
    printf("%s\n", name);
  } 
  else if (st.type == T_DIR) {
    char *name = path;
    for(p=path+strlen(path); p >= path && *p != '/'; p--);
    if (*p == '/') name = p + 1;

    fmt_name(path, depth);
    printf("%s/\n", name);

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("tree: path too long\n");
    } else {
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';

      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0; 
        tree(buf, depth + 1); 
      }
    }
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    tree(".", 0);
  } else {
    tree(argv[1], 0);
  }
  exit(0);
}