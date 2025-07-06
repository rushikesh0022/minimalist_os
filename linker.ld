ENTRY(_start)

SECTIONS
{
    . = 0x40080000;  /* Standard load address for QEMU virt machine */
    
    .text : {
        _start = .;
        *(.text.boot)
        *(.text)
    }
    
    .rodata : {
        *(.rodata)
    }
    
    .data : {
        *(.data)
    }
    
    . = ALIGN(16);
    __bss_start = .;
    .bss : {
        *(.bss)
    }
    __bss_end = .;
    
    . = ALIGN(4096);
    heap_start = .;
    . = . + 0x100000;  /* Reserve 1MB for heap */
    heap_end = .;

    /DISCARD/ : {
        *(.comment)
        *(.gnu*)
    }
}
