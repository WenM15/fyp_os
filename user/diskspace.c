#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(void)
{
    uint free_blocks, total_blocks;
    if(diskspace(&free_blocks, &total_blocks) < 0){
        fprintf(2, "diskspace: error getting disk free information\n");
        exit(1);
    }
    
    printf("Total blocks: ");
    printint(1, total_blocks, 10, 0);
    printf(", Free blocks: ");
    printint(1, free_blocks, 10, 0);
    printf("\n");

    exit(0);
}
