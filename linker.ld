ENTRY(_start)

SECTIONS
{
    . = 1M;

		multiboot_header = .;
    .text : {
        *(.multiboot)
				kernel_text = .;
        *(.text*)
    }

		kernel_rodata = .;
    .rodata : { *(.rodata*) }
		kernel_data = .;
    .data   : { *(.data*) }
		kernel_bss = .;
    .bss    : { *(.bss*) }
		kernel_end = .; 
}
