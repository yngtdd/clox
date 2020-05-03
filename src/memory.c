#include <stdlib.h>
#include "common.h"
#include "memory.h"

/*
 * Reallocate memory when growing chunk sizes
 *
 * Handles dynamic memory management for Clox - allocating 
 * memory, freeing memory, and changing the size of an exisiting
 * allocation. It is important that these options be routed through
 * a single function for our garbage collector, which will need to 
 * keep track of how much memory is in use. There are four cases to
 * consider here depending on the two size parameters of this function.
 * 
 * Reallocation cases:
 *
 * | size_old | size_new            | operation                  |
 * |----------|---------------------|----------------------------|
 * | 0        | non-zero            | Allocate new block         |
 * | non-zero | 0                   | Free allocation            |
 * | non-zero | size_new < size_old | Shrink existing allocation |
 * | non-zero | size_new > size_old | Grow existing allocation   |
 */
void* reallocate(void* previous, size_t, size_old, size_new)
{
    if (size_new == 0)
    {
        free(previous);
        return NULL;
    }

    return realloc(previous, size_new);
}
