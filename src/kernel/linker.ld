ENTRY(kernel_entry)

SECTIONS {
    . = 0xFFFF800000100000;
    .text : { *(.text*) }
    .data : { *(.data*) }
    .bss  : { *(.bss*)  }
}
