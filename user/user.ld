ENTRY(_start)
SECTIONS {
    . = 0x400000;
    .text : {
        user/user_program_asm.o(.text*)
        *(.text*)
    }
    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }
    .bss    : { *(.bss*) }
}
