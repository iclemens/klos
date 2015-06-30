ENTRY(start)

SECTIONS
{
	.text 0xC0000000 : AT(0x100000)
	{
		*(.text)
		. = ALIGN(4096);
	}

	.rodata : { 
		*(.rodata) 
		*(.rodata.*) 
		. = ALIGN(4096);
	}

	.data : 
	{
		*(.data)
		. = ALIGN(4096);
	}

	.bss : 
	{
		*(.bss)
		. = ALIGN(4096);
	}

	end_of_kernel = .;
}

