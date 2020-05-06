#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "debug.h"
#include "common.h"
#include "vm.h"

/**
 * Run the REPL
 *
 * This is a super simple REPL. We do not 
 * support multiline interpretation and we 
 * have a hard limit on line length (1024).
 */
static void repl()
{
    char line[1024];
    for (;;)
    {
        printf("clox > ");

        if (!fgets(line, sizeof(line), stdin))
        {
            printf("\n");
            break;
        }

        vm_interpret(line);
    }
}

/**
 * Read source code from a file
 */
static char* file_read(const char* path)
{
    //TODO(TODD): error checking 
    FILE* file = fopen(path, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);

    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);<Paste>
        exit(74);
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);

    if (bytes_read < file_size)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path); 
        exit(74)
    }

    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

/**
 * Run source code from a file
 */
static void file_run(const char* path)
{
    char* source = file_read(path);
    InterpretResult result = vm_interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) 
{
    vm_init();

    if (argc == 1)
    {
        repl();
    }
    else if (argc == 2)
    {
        file_run(argv[1]);
    }
    else
    {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }

    vm_free();
    return 0;
}
