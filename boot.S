.section ".text.boot"
.global _start

_start:
    // Check processor ID, halt secondary cores
    mrs     x0, mpidr_el1
    and     x0, x0, #3
    cbz     x0, 2f
1:  wfe
    b       1b
2:  // Primary CPU continues

    // Set up stack pointer
    ldr     x0, =_start
    mov     sp, x0

    // Clear BSS
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end
    sub     x1, x1, x0
    bl      memzero

    // Jump to C code
    bl      kernel_main

    // If we return, halt
    b       1b

// Simple memzero routine
memzero:
    // x0 = start address, x1 = size
    cbz     x1, .Lmzend
    str     xzr, [x0], #8
    sub     x1, x1, #8
    cbnz    x1, memzero
.Lmzend:
    ret
