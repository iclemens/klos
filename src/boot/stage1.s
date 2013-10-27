[ORG 0x7C00]
[BITS 16]

; Jump to loader
jmp 0x0:loader
nop

; Store amount of memory for later
memory dd 0

; Bootloader
loader:

	; Clear interrupts
	cli

	mov ax, 0x9000
	mov ss, ax
	mov sp, 0xFFFF

	lgdt [gdt_pointer_32]
	lidt [idt_pointer_32]	

	; Enable protected mode
	mov eax, cr0
	or al, 1
	mov cr0, eax

	jmp dword 0x08:loader_32


[BITS 32]
loader_32:
	; Setup segments
	mov eax, 0x10
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	a32

	; Put a letter on the screen
	mov eax, 0x10000
	mov ebx, 0xB8000

	mov cl, 0x41
	mov byte[ebx], cl

	tmp:
	jmp dword 0x08:tmp


;
; GDT and IDT tables
;

idt_pointer_32:
	dw 0x00
	dw 0x00, 0x00

gdt_pointer_32:
	dw 0x17
	dd gdt_data_32

gdt_data_32:
	; 0x00 NULL descriptor
	dw 0, 0, 0, 0

	; 0x08 Code segment
	dw 0xFFFF ; Segment limit
	dw 0x0000 ; Segment base 1
	dw 0x9800 ; Flags / base 2
	dw 0x00CF ; Base 3 / Flags

	; 0x10 Data segment
	dw 0xFFFF
	dw 0x0000
	dw 0x9200
	dw 0x00CF

; Write magic value at end of sector
times 510 - ($ - $$) db 0
dw 0xAA55

