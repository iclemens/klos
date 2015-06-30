[BITS 32]

;
; Kernel bootstrap
;
; This code, which should be located at physical address 0x100000
; initializes paging and creates an identity table such that
; the virtual address 0x100000 points to the physical address 0x100000.
; This is nessecary because otherwise the EIP would be invalid after
; enabling paging. In addition the virtual address 0xC0000000 also
; maps to the physical address 0x100000. Then paging is intialized and
; a jump will be made to 0xC0000000. This makes the identity map of the
; lower 1MB unnessecary.
;
; Note that this bootstrap code does NOT pass-on multiboot info.
; The multiboot info will be lost.
;

[global start]
;[global loop]

[extern main]
[extern end_of_kernel]

;
; Multiboot header
;
; Source: http://www.openbg.net/sto/os/xml/grub.html
;

MULTIBOOT_PAGE_ALIGN 		equ 1 << 0
MULTIBOOT_MEMORY_INFO 	equ 1 << 1

MULTIBOOT_HEADER_MAGIC 	equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS 	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
CHECKSUM 								equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

align 4
dd MULTIBOOT_HEADER_MAGIC
dd MULTIBOOT_HEADER_FLAGS
dd CHECKSUM

;
; The entry point
;

start:	
	push dword [ebx + 8]
	push dword [ebx + 4]

	; Calculate address of page immediately after kernel
	mov ebx, end_of_kernel
	sub ebx, 0xBFF00000
	and ebx, 0xFFFFF000
	add ebx, 0x00001000

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Create blank page directory
	mov edi, ebx
	xor eax, eax
	mov eax, 0x02
	mov ecx, 1024

	loopdir:
		stosd
		loop loopdir

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Create pointer table
	mov edi, ebx
	add edi, 0x1000
	mov eax, 0x00
	mov ecx, 1024

	looptabl2:
		stosd
		loop looptabl2

	; The kernel page_table
	mov eax, 0xC03FE000
	mov dword[ebx + 0x1C00], eax

	; The identity page_table
	mov eax, 0xC03FF000
	mov dword[ebx + 0x1000], eax	

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Create 0xC0000000 = 0x00100000 pagetable
	mov edi, ebx
	add edi, 0x2000
	mov eax, 0x100000
	or  eax, 0x03
	mov ecx, 1024

	looptabl1:
		stosd
		add eax, 0x1000
		loop looptabl1

	; The page directory occupies the fourth last page in the kernel page table
	mov eax, ebx
	or eax, 0x03
	mov dword[ebx +  0x2FF0], eax

	; The pointer table occupies the third last page in the kernel page table
	mov eax, ebx
	or eax, 0x03
	add eax, 0x1000;
	mov dword[ebx +  0x2FF4], eax

	; The kernel page table occupies the second last page in the kernel page table
	mov eax, ebx
	or eax, 0x03
	add eax, 0x2000;
	mov dword[ebx +  0x2FF8], eax

	; The identity page table occupies the  last page in the kernel page table
	mov eax, ebx
	or eax, 0x03
	add eax, 0x3000;
	mov dword[ebx +  0x2FFC], eax

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Create identity pagetable
	mov edi, ebx
	add edi, 0x3000
	xor eax, eax
	or  eax, 0x03
	mov ecx, 1024

	looptabl0:
		stosd
		add eax, 0x1000
		loop looptabl0

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Load directory!

	; The kernel page_table
	mov eax, ebx
	add eax, 0x2000
	or  eax, 0x03
	mov dword[ebx + 0xC00], eax

	; The identity page_table
	mov eax, ebx
	add eax, 0x3000
	or  eax, 0x03
	mov dword[ebx], eax

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Load directory address and enable paging
	mov eax, ebx
	mov cr3, eax

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	pop dword ebx
	pop dword eax

	; This jump invalidates the prefetch queue
	jmp 0x8:start_c_kernel

; Setup a small stack and call the main function
start_c_kernel:
	mov esp, _stack

	push eax
	push ebx

	; Clear debug registers, just to be sure
	mov eax, 0x0000
	mov dr6, eax
	mov dr7, eax

	call main
	jmp $							; Hang, might the kernel return (which should not happen)

; Reserved for the stack. Note that the stack moves downwards
; It might be better to move the stack to it's own pages in
; virtual memory.

SECTION .bss
	resb 8092
_stack:
