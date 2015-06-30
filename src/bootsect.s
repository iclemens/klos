[ORG 0x7C00]
[BITS 16]

;
; This is the boot loader
; It loads the 2048 directly after the bootloader into memory at 0x100000 and
; transfers control to it.
;
jmp 0x0:loader
nop

; Boot parameters (not really required, this is a DOS thing)
boot_drive		db 0
memory			dd 0

filler1			db '  '

bytes_per_sector	dw 512	; 0x0B

lba			dw 1
sector			db 1
cylinder 		db 1
head 			db 1

filler2			db '      '

sectors_per_track	dw 18		; 0x18
number_of_heads 	dw 2		; 0x1A

loader:
	mov [boot_drive], dl

	; Get memory size from BIOS
	mov ax, 0xE801
	int 0x15
	jc $

	call enable_a20			; Enable A20 Line, Should provide backup

	mov ax, 0x1000
	mov es, ax

	; Start address
	mov ax, 9		; Should be 1, but skip 4096 bytes / 512 bytes
	mov word[lba], ax

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; LOOP
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	load_next_sector:

	call lba_to_chs

	; Set offset for data-buffer
	xor ebx, ebx
	mov bx, word[lba]
	sub bx, 9
	shl bx, 9

	call load_sector

	mov eax, [cylinder]
	call print_numeral
	mov eax, [head]
	call print_numeral
	mov eax, [sector]
	call print_numeral

	; Increment LBA
	xor eax, eax
	mov ax, word[lba]
	inc ax
	mov word[lba], ax

	cmp ax, 50 ;2881			; Reads the entire floppy
	jne load_next_sector

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; END LOOP
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Turn off floppy motor...
	mov dx, 0x3F2
	xor al, al
	out dx, al

	; Clear interrupts
	cli

	mov ax, 0x9000
	mov ss, ax
	mov sp, 0xFFFF

	xor ax, ax
	mov ds, ax

	lgdt [gdt_pointer]
	lidt [idt_pointer]

	; Enable protected mode
	mov eax, cr0
	or al, 1
	mov cr0, eax

	jmp dword 0x08:loader_32

; Converts LBA to CHS
lba_to_chs:
	mov ax, word[lba]				; Assume AX contains the LBA address
	xor dx, dx
	div word[sectors_per_track]		; AX:DX / word[sec...]

	; AX = Quotient (num_tracks)
	; DX = Remainder

	inc dx
	mov [sector], dl

	xor dx, dx
	div word[number_of_heads]

	mov [cylinder], al
	mov [head], dl

	ret

; Loads a sector from disk into memory...
; Note that the BIOS is being used here,
;  this should be changed.
load_sector:
	; Reset disk system
	mov ah, 0
	mov dl, [boot_drive]
	int 0x13

	mov ah, 0x02 		; Read sector
	mov al, 0x01 		; Read ONE sector
	mov ch, [cylinder] 	; Cylinder to read
	mov cl, [sector] 	; Sector to read
	mov dh, [head] 		; Head to read
	mov dl, [boot_drive]
	
	int 0x13
						; Call the BIOS
	ret

print_numeral:
	mov ah, 0
	mov dl, 16
	div dl

	mov bx, ax

	mov ah, 0x0e
	mov al, bl
	add al, 65
	int 0x10

	mov ah, 0x0e
	mov al, bh
	add al, 65
	int 0x10	

	mov ah, 0x0e
	mov al, ')'
	int 0x10

	ret

failure:
	mov ax, 0xB800
	mov es, ax
	mov al, 66
	mov ah, 7
	mov [es:0], ax
	jmp failure

enable_a20:
	call wait_kb
	mov al, 0xd1
	out 0x64, al
	call wait_kb
	jnz failure
	mov al, 0xdf
	out 0x60, al
	call wait_kb
	ret

wait_kb:
	in al, 0x64
	test al, 2
	jnz wait_kb
	ret

[BITS 32]

;
; The 32 bit bootloader
;

loader_32:
	mov eax, 0x10
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	a32

	mov eax, 0x10000
	mov ebx, 0xB8000

	printmore:

	mov cl, byte[eax]
	mov byte[ebx], cl
	add ebx, 2
	add eax, 1

	cmp eax, 0x10100
	jle printmore

	;cli
	;hlt

	; Start kernel
	jmp dword 0x8:0x1000C
	;jmp dword ptr 0x8:[eax]

	;jmp dword 0x8:0xC0000000

; Wait for a keypress
wait_kb_32:
	in al, 0x64
	test al, 0x01
	jz wait_kb_32

	in al, 0x60
	cmp al, 0x01
	jne wait_kb_32

	ret

;
; GDT and IDT Tables
;
; Those are only used for initialization
;  real GDT/IDT tables are provided later
;

idt_pointer:
	dw 0x00
	dw 0x00, 0x00

gdt_pointer:
	dw 0x17
	dd gdt_data

gdt_data:
	; 0x00 (null descriptor)
	dw 0, 0, 0, 0

	; 0x08 (code segment)
	dw 0xFFFF		; Segment limit
	dw 0x0000		; Segment base 1|0
	dw 0x9800		; Flags | Base 2
	dw 0x00CF		; Base 3 | Flags

	; 0x10 (data segment)
	dw 0xFFFF
	dw 0x0000
	dw 0x9200
	dw 0x00CF

times 510-($-$$) db 0
dw 0xAA55