#include "chunk.h"
#include "debug.h"
#include "common.h"
#include "vm.h"

void append_constant(Chunk* chunk, double num, int line)
{
    int constant = chunk_constant_add(chunk, num);
    chunk_write(chunk, OP_CONSTANT, line);
    chunk_write(chunk, constant, line);
}

int main(int argc, const char* argv[]) 
{
    vm_init();

    Chunk chunk;
    chunk_init(&chunk);

    append_constant(&chunk, 2, 20);
    append_constant(&chunk, 3, 21);

    chunk_write(&chunk, OP_MULTIPLY, 23);

    append_constant(&chunk, 4, 22);
    append_constant(&chunk, 5, 22);
    chunk_write(&chunk, OP_NEGATE, 27);
    chunk_write(&chunk, OP_DIVIDE, 26);
    chunk_write(&chunk, OP_SUBTRACT, 29);
    append_constant(&chunk, 1, 30);
    chunk_write(&chunk, OP_ADD, 31);

    chunk_write(&chunk, OP_RETURN, 28);

    vm_interpret(&chunk);

    vm_free();
    chunk_free(&chunk);

    return 0;
}
