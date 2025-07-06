#include "uart.h"

// QEMU PL011 UART registers
#define UART_BASE 0x09000000
#define UART_DR (*((volatile uint32_t *)(UART_BASE + 0x00)))
#define UART_FR (*((volatile uint32_t *)(UART_BASE + 0x18)))
#define UART_FR_TXFF (1 << 5)
#define UART_FR_RXFE (1 << 4)

void uart_init(void)
{
    // The PL011 UART is already initialized by QEMU
}

void uart_putc(char c)
{
    // Wait until there is space in the FIFO
    while (UART_FR & UART_FR_TXFF)
        ;

    // Send character
    UART_DR = c;

    // If it's a newline, also send carriage return
    if (c == '\n')
    {
        uart_putc('\r');
    }
}

void uart_puts(const char *str)
{
    while (*str)
    {
        uart_putc(*str++);
    }
}

char uart_getc(void)
{
    // Wait until there is data in the FIFO
    while (UART_FR & UART_FR_RXFE)
        ;

    // Return the character
    return (char)UART_DR;
}

void uart_print_int(int value)
{
    char buffer[12]; // Large enough for 32-bit int including sign and null
    char *p = buffer + sizeof(buffer) - 1;
    *p = '\0';

    int is_negative = value < 0;
    if (is_negative)
    {
        value = -value;
    }

    do
    {
        *(--p) = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    if (is_negative)
    {
        *(--p) = '-';
    }

    uart_puts(p);
}

void uart_print_hex(uint64_t value)
{
    uart_puts("0x");

    int started = 0;
    for (int i = 60; i >= 0; i -= 4)
    {
        uint8_t digit = (value >> i) & 0xF;
        if (digit || started || i == 0)
        {
            started = 1;
            uart_putc(digit < 10 ? '0' + digit : 'A' + digit - 10);
        }
    }
}

void uart_gets(char *buffer, int max_len)
{
    int i = 0;
    char c;

    while (i < max_len - 1)
    {
        c = uart_getc();

        // Echo the character
        uart_putc(c);

        if (c == '\r' || c == '\n')
        {
            uart_puts("\n");
            break;
        }
        else if (c == '\b' && i > 0)
        {
            // Handle backspace
            i--;
            uart_puts(" \b"); // Erase character
        }
        else if (c >= ' ' && c <= '~')
        { // Printable characters
            buffer[i++] = c;
        }
    }

    buffer[i] = '\0';
}
