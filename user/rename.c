#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "Usage: rename oldname newname\n");
    exit(1);
  }
  
  if(rename(argv[1], argv[2]) < 0){
    fprintf(2, "rename %s %s failed\n", argv[1], argv[2]);
    exit(1);
  }
  exit(0);
}
