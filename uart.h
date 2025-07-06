#ifndef _UART_H
#define _UART_H

// Define types that would normally be in stdint.h
typedef unsigned long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

// Initialize UART
void uart_init(void);

// Send a single character
void uart_putc(char c);

// Send a string
void uart_puts(const char *str);

// Read a character (blocking)
char uart_getc(void);

// Print an integer value
void uart_print_int(int value);

// Print a hexadecimal value
void uart_print_hex(uint64_t value);

// Read a line from UART (up to max_len-1 chars, null-terminated)
void uart_gets(char *buffer, int max_len);

#endif /* _UART_H */
