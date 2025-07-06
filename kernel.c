#include "uart.h"
#include "allocator.h"

// Define types that would normally be in standard headers
typedef unsigned long size_t;
typedef unsigned long uint64_t;

// Simple string comparison
int str_equals(const char *s1, const char *s2, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0')
        {
            return 0;
        }
    }
    return 1;
}

// Parse decimal number from string
int parse_int(const char *str)
{
    int result = 0;

    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result;
}

// Parse hex number from string (0x prefix optional)
uint64_t parse_hex(const char *str)
{
    uint64_t result = 0;

    // Skip "0x" prefix if present
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        str += 2;
    }

    while (1)
    {
        char c = *str;
        if (c >= '0' && c <= '9')
        {
            result = result * 16 + (c - '0');
        }
        else if (c >= 'a' && c <= 'f')
        {
            result = result * 16 + (c - 'a' + 10);
        }
        else if (c >= 'A' && c <= 'F')
        {
            result = result * 16 + (c - 'A' + 10);
        }
        else
        {
            break;
        }
        str++;
    }

    return result;
}

void kernel_main(void)
{
    // Initialize UART
    uart_init();

    // Welcome message
    uart_puts("\n\n===================================\n");
    uart_puts("ARM64 Bare Metal Memory Allocator\n");
    uart_puts("===================================\n\n");

    // Initialize memory allocator
    init_allocator();

    // Command buffer
    char cmd[128];

    // Shell loop
    uart_puts("\nShell ready. Available commands:\n");
    uart_puts("  alloc <size> - Allocate memory of <size> bytes\n");
    uart_puts("  free <addr> - Free memory at address <addr>\n");
    uart_puts("  info - Display memory allocator information\n");
    uart_puts("  help - Display this help message\n\n");

    while (1)
    {
        uart_puts("> ");
        uart_gets(cmd, sizeof(cmd));

        // Process commands
        if (str_equals(cmd, "help", 4))
        {
            uart_puts("Available commands:\n");
            uart_puts("  alloc <size> - Allocate memory of <size> bytes\n");
            uart_puts("  free <addr> - Free memory at address <addr>\n");
            uart_puts("  info - Display memory allocator information\n");
            uart_puts("  help - Display this help message\n");
        }
        else if (str_equals(cmd, "info", 4))
        {
            print_allocator_info();
        }
        else if (str_equals(cmd, "alloc ", 6))
        {
            int size = parse_int(cmd + 6);
            if (size <= 0)
            {
                uart_puts("Error: Invalid size\n");
                continue;
            }

            void *ptr = my_malloc(size);

            if (ptr)
            {
                uart_puts("Allocated ");
                uart_print_int(size);
                uart_puts(" bytes at address ");
                uart_print_hex((uint64_t)ptr);
                uart_puts("\n");
            }
            else
            {
                uart_puts("Error: Failed to allocate memory\n");
            }
        }
        else if (str_equals(cmd, "free ", 5))
        {
            uint64_t addr = parse_hex(cmd + 5);
            if (addr == 0)
            {
                uart_puts("Error: Invalid address\n");
                continue;
            }

            uart_puts("Freeing memory at ");
            uart_print_hex(addr);
            uart_puts("\n");

            my_free((void *)addr);
        }
        else
        {
            uart_puts("Unknown command: ");
            uart_puts(cmd);
            uart_puts("\n");
        }
    }
}
