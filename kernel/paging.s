[BITS 32]

[global swap_pagetable]

swap_pagetable:
	push eax
	mov eax, [esp + 8]
	mov cr3, eax

	jmp 0x8:invalidate_pfq
	invalidate_pfq:
	pop eax
	ret
