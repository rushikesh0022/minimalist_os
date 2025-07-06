#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

// Define size_t for bare metal environment
typedef unsigned long size_t;

// Initialize the memory allocator
void init_allocator(void);

// Allocate memory of given size
void *my_malloc(size_t size);

// Free previously allocated memory
void my_free(void *ptr);

// Print allocator statistics
void print_allocator_info(void);

#endif /* _ALLOCATOR_H */
