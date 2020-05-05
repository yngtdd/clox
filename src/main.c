#include "chunk.h"
#include "debug.h"
#include "common.h"
#include "vm.h"

int main(int argc, const char* argv[]) 
{
    vm_init();

    Chunk chunk;
    chunk_init(&chunk);

    int constant = chunk_constant_add(&chunk, 3.4);
    chunk_write(&chunk, OP_CONSTANT, 14);
    chunk_write(&chunk, constant, 15);
    chunk_write(&chunk, OP_NEGATE, 16);
    chunk_write(&chunk, OP_RETURN, 17);

    vm_interpret(&chunk);

    vm_free();
    chunk_free(&chunk);

    return 0;
}
