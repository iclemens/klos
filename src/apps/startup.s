[BITS 32]

;
; Application startup routine, linked to every application
;

[global startup]
[extern main]

startup:

;mov esp, _stack

call main

; Exit with error code 0
mov eax, 1
mov ebx, 0
int 0x80

jmp $

; Reserved for the stack. Note that the stack moves downwards
; It might be better to move the stack to it's own pages in
; virtual memory.

SECTION .bss
	resb 8092
_stack:
