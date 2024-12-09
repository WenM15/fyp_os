#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
fileinfo(char *path)
{
  struct stat st;
  if(stat(path, &st) < 0){
    printf("fileinfo: cannot stat %s\n", path);
    return -1;
  }
  printf("File: %s\nSize: %d\nLinks: %d\nInode: %d\nType: ", path, st.size, st.nlink, st.ino);
  switch(st.type){
    case T_DIR: printf("directory\n"); break;
    case T_FILE: printf("file\n"); break;
    case T_DEVICE: printf("device\n"); break;
    default: printf("unknown\n");
  }
  return 0;
}

int
main(int argc, char *argv[])
{
  if(argc != 2){
    printf("Usage: fileinfo <path>\n");
    exit(1);
  }
  if(fileinfo(argv[1]) < 0)
    exit(1);
  exit(0);
}
