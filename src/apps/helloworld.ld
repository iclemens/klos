OUTPUT_FORMAT("binary")
ENTRY(startup)

SECTIONS
{
	.text 0x0 : AT(0x0) 
	{
		*(.text)
	}

	.rodata : { *(.rodata) *(.rodata.*) }

	.data : 
	{
		*(.data)
	}

	.bss : 
	{
		*(.bss)
	}

	end_of_application = .;
}

