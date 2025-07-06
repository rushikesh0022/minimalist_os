#include "allocator.h"
#include "uart.h"

// Define NULL for bare metal environment
#define NULL ((void *)0)

// Defined in linker script
extern char heap_start;
extern char heap_end;

// Block header structure
typedef struct block_header
{
    size_t size;
    int is_free;
    struct block_header *next;
} block_header_t;

static block_header_t *head = NULL;
static size_t total_allocated = 0;
static size_t total_free = 0;

void init_allocator(void)
{
    head = (block_header_t *)&heap_start;
    head->size = (size_t)(&heap_end - &heap_start) - sizeof(block_header_t);
    head->is_free = 1;
    head->next = NULL;

    total_allocated = 0;
    total_free = head->size;

    uart_puts("Memory allocator initialized\n");
    uart_puts("Heap start: ");
    uart_print_hex((uint64_t)&heap_start);
    uart_puts("\nHeap end: ");
    uart_print_hex((uint64_t)&heap_end);
    uart_puts("\nTotal heap size: ");
    uart_print_int(head->size);
    uart_puts(" bytes\n");
}

void *my_malloc(size_t size)
{
    // Align size to 8 bytes
    size = (size + 7) & ~7;

    // Find a free block that is big enough
    block_header_t *current = head;

    while (current)
    {
        if (current->is_free && current->size >= size)
        {
            // Check if we should split the block
            if (current->size >= size + sizeof(block_header_t) + 8)
            {
                // Create a new block after this allocation
                block_header_t *new_block = (block_header_t *)((char *)current + sizeof(block_header_t) + size);
                new_block->size = current->size - size - sizeof(block_header_t);
                new_block->is_free = 1;
                new_block->next = current->next;

                // Update current block
                current->size = size;
                current->next = new_block;

                total_free -= (size + sizeof(block_header_t));
            }
            else
            {
                total_free -= current->size;
            }

            current->is_free = 0;
            total_allocated += current->size;

            // Return pointer to the allocated memory (after the header)
            return (void *)((char *)current + sizeof(block_header_t));
        }

        current = current->next;
    }

    // No suitable block found
    return NULL;
}

void my_free(void *ptr)
{
    if (!ptr)
    {
        return;
    }

    // Get the block header
    block_header_t *block = (block_header_t *)((char *)ptr - sizeof(block_header_t));

    // Mark as free
    if (!block->is_free)
    {
        block->is_free = 1;
        total_allocated -= block->size;
        total_free += block->size;

        // Simple coalescing with next block if it's free
        if (block->next && block->next->is_free)
        {
            block->size += sizeof(block_header_t) + block->next->size;
            block->next = block->next->next;
            total_free += sizeof(block_header_t);
        }
    }
    else
    {
        uart_puts("Error: Double free detected\n");
    }
}

void print_allocator_info(void)
{
    uart_puts("Memory Allocator Info:\n");
    uart_puts("  Total heap size: ");
    uart_print_int((int)(&heap_end - &heap_start));
    uart_puts(" bytes\n");
    uart_puts("  Total allocated: ");
    uart_print_int(total_allocated);
    uart_puts(" bytes\n");
    uart_puts("  Total free: ");
    uart_print_int(total_free);
    uart_puts(" bytes\n");

    // List all blocks
    uart_puts("Memory blocks:\n");
    block_header_t *current = head;
    int block_count = 0;

    while (current)
    {
        uart_puts("  Block ");
        uart_print_int(block_count++);
        uart_puts(": Address=");
        uart_print_hex((uint64_t)current);
        uart_puts(", Size=");
        uart_print_int (current->size);
        uart_puts(", Status=");
        uart_puts(current->is_free ? "FREE" : "USED");
        uart_puts("\n");

        current = current->next;
    }
}
