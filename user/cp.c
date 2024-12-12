#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "user.h"

char buf[512];

void
cp(char *src, char *dest)
{
    int srcfd, destfd, n;

    // Open source file for reading
    if((srcfd = open(src, O_RDONLY)) < 0){
        fprintf(2, "cp: cannot open %s\n", src);
        exit(1);
    }

    // Create or truncate destination file for writing
    if((destfd = open(dest, O_WRONLY|O_CREATE|O_TRUNC)) < 0){
        close(srcfd);
        fprintf(2, "cp: cannot create or truncate %s\n", dest);
        exit(1);
    }

    // Copy data from source to destination
    while((n = read(srcfd, buf, sizeof(buf))) > 0){
        if(write(destfd, buf, n) != n){
            fprintf(2, "cp: write error\n");
            close(srcfd);
            close(destfd);
            exit(1);
        }
    }

    if(n < 0){
        printf("cp: read error\n");
    }

    close(srcfd);
    close(destfd);
}

int
main(int argc, char *argv[])
{
    if(argc != 3){
        fprintf(2, "Usage: cp src dest\n");
        exit(1);
    }

    struct stat st;
    if(stat(argv[1], &st) < 0){
      fprintf(2, "cp: cannot get info on %s\n", argv[1]);
      exit(1);
    }
    if (st.type == T_DIR || st.type == T_DEVICE){
      fprintf(2, "cp: %s is either a directory or device file, cannot copy.\n", argv[1]);
      exit(1);
    }

    if(stat(argv[2], &st) < 0){
      fprintf(2, "cp: cannot get info on %s\n", argv[2]);
      exit(1);
    }
    if (st.type == T_DIR || st.type == T_DEVICE){
      fprintf(2, "cp: %s is either a directory or device file, cannot copy.\n", argv[2]);
      exit(1);
    }

    cp(argv[1], argv[2]);
    exit(0);
}
