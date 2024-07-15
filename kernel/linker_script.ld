OUTPUT_ARCH("riscv")
ENTRY(entry_point)

SECTIONS
{
	. = 0x80000000;

	.text : {
		*(.text)
		
		PROVIDE(end_text = .);
	}

	.rodata : {
		. = ALIGN(16);
		*(.rodata)
	}

	.data : {
		. = ALIGN(16);
		*(.data)
	}

	.bss : {
		. = ALIGN(16);
		*(.bss)
	}

	PROVIDE(end = .);
}
