#include "kernel/types.h"
#include "user.h"

void consume_stack(int depth) {
    char large_array[4096]; // takes up one page of space
    large_array[0] = depth;
    if (depth > 0) {
        fprintf(1, "Depth: %d, Array[0]: %d\n", depth, large_array[0]);
        consume_stack(depth - 1);
    }
}

int main() {
    consume_stack(4);
    fprintf(1, "Finished consuming stack\n");
    return 0;
}
