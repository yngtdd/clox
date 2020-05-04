#include "chunk.h"
#include "debug.h"
#include "common.h"

int main(int argc, const char* argv[]) 
{
    Chunk chunk;
    chunk_init(&chunk);

    for (int i = 0; i < 5; i++)
    {
        chunk_write(&chunk, OP_RETURN, 12);
    }

    int constant = chunk_constant_add(&chunk, 1.2);
    chunk_write(&chunk, OP_CONSTANT, 16);
    chunk_write(&chunk, constant, 17);

    chunk_write(&chunk, OP_RETURN, 19);

    chunk_disassemble(&chunk, "Test Chunk");
    chunk_free(&chunk);

    return 0;
}
