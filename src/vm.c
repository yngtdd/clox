#include <stdio.h>
#include "common.h"
#include "debug.h"
#include "value.h"
#include "vm.h"

VM vm;

void vm_init() 
{
    stack_reset();
}

/**
 * Reset the virtual machien stack
 *
 * This sets the stack pointer `stack_top`
 * to be pointed at the begging of the stack
 * array, indicating that the stack is empty.
 */
static void stack_reset()
{
    vm.stack_top = vm.stack;
}

void vm_free() {}

/**
 * Push to the top of the virtual machine stack
 *
 * Appends a value to the top of the stack (where 
 * the `stack_top` pointer currently resides) and 
 * moves the stack top pointer up one index.
 */
void vm_stack_push(Value value)
{
    *vm.stack_top = value;
    vm.stack_top++;
}

/**
 * Pop from the top of the virutal machine stack
 *
 * Move the `stack_top` pointer back one step and
 * get the value at that location.
 */
Value vm_stack_pop()
{
    vm.stack_top--;
    return *vm.stack_top;
}

/*
 * Interpret the code
 *
 * @param chunk the chunk of memory
 *
 * The virtual machine will make its way through
 * the bytecode, keeping track of where it is. We
 * keep track of the what instruction is being run
 * with `vm.ip`, a byte pointer commonly known as an
 * instruction pointer. This is also commonly referred
 * to as a program counter.
 */
InterpretResult vm_interpret(Chunk* chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return vm_run();
}

/**
 * Run the interpretation
 *
 * The beating heart of Clox. This is where the
 * the code will spend 90% of its time. Loop continuously
 * reading and executing a single bytecode at a time.
 */
static InterpretResult vm_run()
{
    #define READ_BYTE() (*vm.ip++)
    // Read the next byte from bytecode, treat the resulting number as an
    // index, and look up the corresponding location in the chunk's constant table.
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

    for (;;)
    {
        #ifdef DEBUG_TRACE_EXECUTION
            instruction_disassemble(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif

        uint8_t instruction;
        switch (instruction = READ_BYTE())
        {
            case OP_CONSTANT:
            {
                Value constant = READ_CONSTANT();
                value_print(constant);
                printf("\n");
                break;
            }
            case OP_RETURN:
            {
                return INTERPRET_OK;
            }
        }
    }

    #undef READ_BYTE
    #undef READ_CONSTANT
}