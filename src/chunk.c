#include <stdlib.h>
#include "chunk.h"
#include "memory.h"

/**
 * Initialize a code chunk
 *
 * Here a chunk is a blob of bytecode where an
 * instruction in the language has a one-byte
 * operation code (opcode).
 */
void chunk_init(Chunk* chunk) 
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
}

/**
 * Free a chunk
 *
 * Deallocate all of the memory of a chunk and 
 * call `chunk_init` to reallocate a chunk leaving the
 * chunk in a well-defined empty state.
 */
void chunk_free(Chunk* chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    chunk_init(chunk);
}

/**
 * Write to our code chunk
 *
 * This will append a new code op to our chunk.
 * If we find that adding a new codeop to the 
 * chunk means that we would be going beyond the
 * capacity of the chunk, we first grow the chunk,
 * doubling the size of the array. We then copy 
 * the elements of the array over to the newly allocated 
 * array. Doubling the size in this way means that on 
 * average, the cost of appending to the chunk is O(1).
 */
void chunk_write(Chunk* chunk, uint8_t byte) 
{
    if (chunk->capacity < chunk->count + 1) 
    {
        int capacity_old = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(capacity_old);
        chunk->code = GROW_ARRAY(
            chunk->code, uint8_t, capacity_old, chunk->capacity
        );
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;
}
