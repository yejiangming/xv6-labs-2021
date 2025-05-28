#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"


void find(char *dir_path, char *target_name) {
  int fd; 
  if((fd = open(dir_path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", dir_path);
    return;
  }

  struct stat st;
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", dir_path);
    close(fd);
    return;
  }

  if (st.type != T_DIR) {
    fprintf(2, "find: %s is not a directory\n", dir_path);
    close(fd);
    return;
  }

  struct dirent de;
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
      continue;
    }
    char buf[512], *p;
    strcpy(buf, dir_path);
    p = buf+strlen(buf);
    *p++ = '/';
    strcpy(p, de.name);
    struct stat sub_st;
    stat(buf, &sub_st);
    if(sub_st.type == T_DIR) {
      find(buf, target_name);
    } else if(sub_st.type == T_FILE) {
      if (strcmp(de.name, target_name) == 0) {
        printf("%s/%s\n", dir_path, de.name);
      }
    }
  }
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: find <dir_path> <name>\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}