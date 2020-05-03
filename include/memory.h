#ifndef clox_memory_h
#define clox_memory_h

/**
 * Grow a chunk's capacity
 *
 * Double the size of the capacity of a chunk
 * given its current capacity. If thechunk is newly 
 * allocated, then it's capacity is zero. In that case, 
 * we start with a capacity of 8. The choice of this
 * starting capacity is somewhat arbitrary. This could be
 * changed based on profiling real world usage.
 */
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

/**
 * Grow a chunk array
 *
 * Reallocate the chunk array with a new size. This will get 
 * the size of the array's element type and will cast the resulting
 * void* back to a pointer of the correct type.
 */
#define GROW_ARRAY(previous, type, count_old, count) \
    (type*)reallocate(previous, sizeof(type) * (count_old), \
        sizeof(type) * (count))

void* reallocate(void* previous, size_t size_old, size_t size_new);

#endif
