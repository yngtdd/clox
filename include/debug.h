#ifndef clox_debug_h
#define clox_debug_h

#include "chunk.h"

void chunk_disassemble(Chunk* chunk, const char* name);
int instruction_disassemble(Chunk* chunk, int offset);

#endif
