#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

  if(argc < 2){
    fprintf(2, "Usage: sleep ticks\n");
    exit(1);
  }

  int n = atoi(argv[1]); // Convert the first argument to an integer
  sleep(n); // Call the sleep function with the number of ticks
  exit(0);
}
