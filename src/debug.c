#include <stdio.h>
#include "debug.h"
#include "value.h"

/**
 * Disassemble all of the instructions in a chunk
 *
 * @param chunk the memory chunk to disassemble
 * @param name the name of the instruction being disassembled
 *
 * This gives us a view into what is going on under the hood
 * whenever a chunk is being called. Since a chunk can have 
 * many instructions in its `code` parameter, we loop through 
 * each of the instructions, calling `instruction_disassemble`
 * to disassemble each instruction.
 */
void chunk_disassemble(Chunk* chunk, const char* name)
{
    printf("== %s ==\n", name);

    // Incrementing offset handled by instruction_disassemble,
    // where it returns the offset of the next instruction for us.
    // This is because instructions can have different sizes.
    for (int offset = 0; offset < chunk->count;) 
    {
        offset = instruction_disassemble(chunk, offset);
    }
}

/**
 * Print a constant instruction
 *
 * @param name the name of the instruction
 * @param chunk the chunk where the value is stored
 * @offset its location in the bytecode
 *
 * After printing the constant value we return the next offset 
 * incrementing by two. This is because the constants store two bytes, 
 * one for the opcode and one for the operand.
 */
static int instruction_constant(const char* name, Chunk* chunk, int offset)
{
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    value_print(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

/**
 * Print a simple instruction name and return next offset
 *
 * @param name the name of the instruction
 * @offset its location in the bytecode
 * @return the offset for the next instruction
 *
 * For simple instructions, we simply print the instruction name
 * and move to the next offset by incrementing the offset by one.
 * Different types of instructions have different offsets, and so
 * the offsets must be handled separately for each type.
 */
static int instruction_simple(const char* name, int offset)
{
    printf("%s\n", name);
    return offset + 1;
}

/**
 * Disassemble an instruction
 *
 * @param chunk the chunk being disassembled 
 * @param offset byte offset of the given instruction
 * @return the instruction in the byte code
 *
 * Disassembling the bytecode instruction allows us to 
 * find what operations are being run at a point in the 
 * bytecode. First we print the byte offset of the given 
 * instruction. We then read a single byte for the bytecode
 * at the given offset. This is our opcode. Finally, if 
 * byte doesn't look like an instruction at all, then we have a 
 * bug in our compiler, and we print that unknown instruction.
 */
int instruction_disassemble(Chunk* chunk, int offset)
{
    printf("%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
    {
        printf("   | ");
    }
    else
    {
        printf("%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];

    switch (instruction)
    {
        case OP_CONSTANT:
            return instruction_constant("OP_CONSTANT", chunk, offset);
        case OP_ADD:
            return instruction_simple("OP_ADD", offset);
        case OP_SUBTRACT:
            return instruction_simple("OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return instruction_simple("OP_MULTIPLY", offset);
        case OP_DIVIDE:
            return instruction_simple("OP_DIVIDE", offset);
        case OP_NEGATE:
            return instruction_simple("OP_NEGATE", offset);
        case OP_RETURN:
            return instruction_simple("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
