#include "chunk.h"
#include "debug.h"
#include "common.h"

int main(int argc, const char* argv[]) 
{
    Chunk chunk;
    chunk_init(&chunk);
    chunk_write(&chunk, OP_RETURN);
    chunk_disassemble(&chunk, "test chunk");
    chunk_free(&chunk);

    return 0;
}
