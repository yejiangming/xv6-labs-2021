#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  char c;

  pid = fork();
  if (pid == 0) {
    c = '/';
  } else {
    c = '\\';
  }

  for (int i = 0; ;i++) {
    if (i % 1000000 == 0) {
      write(1, &c, 1);
    }
  } 
}
