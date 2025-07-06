# ARM64 Bare Metal Memory Allocator

A custom memory allocator implementation for ARM64 bare-metal systems, featuring a minimal operating system kernel with UART communication and interactive shell.

## Overview

This project implements a complete bare-metal system that includes:

- **Custom Memory Allocator**: A heap-based memory management system with block splitting and coalescing
- **ARM64 Kernel**: Minimal kernel that boots on ARM64 hardware/emulation
- **UART Communication**: Serial communication for input/output
- **Interactive Shell**: Command-line interface for testing the allocator
- **Linker Script**: Custom memory layout for bare-metal execution

## Features

### Memory Allocator

- **Dynamic allocation**: `malloc`-like functionality with `my_malloc()`
- **Memory deallocation**: `free`-like functionality with `my_free()`
- **Block splitting**: Efficiently splits large blocks when allocating smaller sizes
- **Block coalescing**: Merges adjacent free blocks to prevent fragmentation
- **Alignment**: All allocations are aligned to 8-byte boundaries
- **Debug information**: Real-time memory statistics and block visualization

### System Features

- **Bare-metal kernel**: No operating system dependencies
- **UART I/O**: Full serial communication stack
- **Interactive shell**: Real-time testing environment
- **Memory safety**: Double-free detection and error handling

## Project Structure

```
my_allocator/
├── allocator.c      # Memory allocator implementation
├── allocator.h      # Allocator header and interface
├── boot.S          # ARM64 assembly boot code
├── kernel.c        # Main kernel and shell implementation
├── uart.c          # UART driver implementation
├── uart.h          # UART driver header
├── linker.ld       # Linker script for memory layout
├── Makefile        # Build configuration
└── README.md       # This file
```

## Prerequisites

### macOS

```bash
# Install ARM64 cross-compilation toolchain
brew install --cask gcc-aarch64-embedded

# Install QEMU emulator
brew install qemu
```

### Linux (Ubuntu/Debian)

```bash
# Install ARM64 cross-compilation toolchain
sudo apt-get install gcc-aarch64-linux-gnu

# Install QEMU
sudo apt-get install qemu-system-arm
```

## Building and Running

### Build the Kernel

```bash
# Clean previous builds
make clean

# Compile the kernel
make

# This creates kernel8.img - the bootable kernel image
```

### Run in QEMU

```bash
# Run the kernel in ARM64 emulation
make run

# Alternative: Run QEMU directly
qemu-system-aarch64 -M virt -cpu cortex-a53 -nographic -kernel kernel8.img
```

### Exit QEMU

To exit the emulator, press: `Ctrl+A` then `X`

## Usage

Once the kernel boots, you'll see:

```
===================================
ARM64 Bare Metal Memory Allocator
===================================

Memory allocator initialized
Heap start: 0x40081000
Heap end: 0x40181000
Total heap size: 1048552 bytes

Shell ready. Available commands:
  alloc <size> - Allocate memory of <size> bytes
  free <addr> - Free memory at address <addr>
  info - Display memory allocator information
  help - Display this help message

>
```

### Available Commands

#### `alloc <size>`

Allocates memory of the specified size in bytes.

```
> alloc 100
Allocated 100 bytes at address: 0x40081018
```

#### `free <address>`

Frees memory at the specified hexadecimal address.

```
> free 0x40081018
Memory freed at address: 0x40081018
```

#### `info`

Displays detailed memory allocator statistics and block information.

```
> info
Memory Allocator Info:
  Total heap size: 1048552 bytes
  Total allocated: 100 bytes
  Total free: 1048452 bytes
Memory blocks:
  Block 0: Address=0x40081000, Size=100, Status=USED
  Block 1: Address=0x40081080, Size=1048452, Status=FREE
```

#### `help`

Shows all available commands and their usage.

## Memory Management Details

### Allocation Algorithm

1. **Size Alignment**: All allocations are aligned to 8-byte boundaries
2. **First Fit**: Searches for the first free block large enough
3. **Block Splitting**: Splits blocks when they're significantly larger than needed
4. **Metadata**: Each allocated block has a header containing size and status information

### Deallocation Algorithm

1. **Validation**: Checks for double-free attempts
2. **Status Update**: Marks block as free and updates statistics
3. **Coalescing**: Merges with adjacent free blocks to reduce fragmentation

### Memory Layout

```
Heap Memory Layout:
[Block Header][User Data][Block Header][User Data]...

Block Header Structure:
- size: Size of the user data area
- is_free: 1 if free, 0 if allocated
- next: Pointer to next block
```

## Technical Details

### Architecture

- **Target**: ARM64 (AArch64) Cortex-A53
- **Memory Model**: 1MB heap starting at `0x40081000`
- **Alignment**: 8-byte alignment for all allocations
- **Endianness**: Little-endian

### Compilation Flags

- `-ffreestanding`: Bare-metal compilation
- `-nostdinc -nostdlib -nostartfiles`: No standard library dependencies
- `-mcpu=cortex-a53`: Target ARM Cortex-A53 processor
- `-Wall -O2`: Warnings enabled, optimization level 2

### Linker Configuration

The `linker.ld` script defines:

- Code placement in memory
- Stack location
- Heap boundaries (`heap_start` and `heap_end` symbols)

## Testing Examples

### Basic Allocation Test

```
> alloc 64
Allocated 64 bytes at address: 0x40081018

> alloc 128
Allocated 128 bytes at address: 0x40081068

> info
Memory Allocator Info:
  Total heap size: 1048552 bytes
  Total allocated: 192 bytes
  Total free: 1048360 bytes
```

### Fragmentation Test

```
> alloc 100
> alloc 200
> free 0x40081018  # Free the first block
> alloc 50         # Should reuse the freed space
> info             # Check memory layout
```

### Error Handling Test

```
> free 0x40081018
> free 0x40081018  # Try to free the same address again
Error: Double free detected
```

## Development

### Adding New Features

1. **New Commands**: Add cases to the command parser in `kernel.c`
2. **Allocator Features**: Extend functions in `allocator.c`
3. **I/O Functions**: Add UART utilities in `uart.c`

### Debugging

- Use `info` command to inspect memory state
- UART output provides real-time feedback
- QEMU monitor commands available with `Ctrl+A, C`

## Performance Characteristics

### Time Complexity

- **Allocation**: O(n) where n is the number of blocks
- **Deallocation**: O(1) for marking free, O(1) for simple coalescing
- **Memory info**: O(n) for traversing all blocks

### Space Complexity

- **Overhead**: `sizeof(block_header_t)` bytes per allocated block
- **Alignment**: Up to 7 bytes padding per allocation
- **Fragmentation**: Minimal due to coalescing strategy

## Limitations

- **Single-threaded**: No concurrency support
- **No advanced coalescing**: Only merges with next block
- **Fixed heap size**: 1MB heap defined at compile time
- **No garbage collection**: Manual memory management only

## Contributing

1. Fork the repository
2. Create a feature branch
3. Test changes in QEMU
4. Submit a pull request

## License

This project is provided as educational material for understanding bare-metal systems programming and memory management.

---

_Built for ARM64 bare-metal systems • Tested on QEMU ARM64 emulation_
