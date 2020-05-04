#include <stdio.h>
#include "memory.h"
#include "value.h"

/**
 * Initialize a value array
 *
 * This dynamic array holds all constants in our 
 * program. It relies on the same dynamic array 
 * functions as chunks.
 */
void value_array_init(ValueArray* array)
{
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

/**
 * Free all memory in a value array
 *
 * We make use of the memory macros defined in 
 * `memory.h` here. The logic is similar to `chunk_free`.
 */
void value_array_free(ValueArray* array)
{
    FREE_ARRAY(Value, array->values, array->capacity);
    value_array_init(array);
}

/**
 * Write a value to the value array
 *
 * Here we add a constant value to our array of values.
 * Similar to chunks, we first check the capacity of the array.
 * If there is not enough storage, we will reallocate the memory
 * for the array with double the previous storage. With amortized
 * analysis, this averages out to be a O(1) cost.
 */
void value_array_write(ValueArray* array, Value value)
{
    if (array->capacity < array->count + 1)
    {
        int capacity_old = array->capacity;
        array->capacity = GROW_CAPACITY(capacity_old);
        array->values = GROW_ARRAY(
            array->values, Value, capacity_old, array->capacity
        );
    }

    array->values[array->count] = value;
    array->count++;
}
