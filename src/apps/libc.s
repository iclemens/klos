[BITS 32]

[global _sys_exit]
[global _sys_fork]
[global _sys_write]
[global _sys_getpid]

_sys_exit:
	push ebp
	mov ebp, esp

	mov eax, 1	; sys_exit
	mov ebx, 0  ; exit code - unused
	int 0x80

	pop ebp
	ret

_sys_fork:
	push ebp
	mov ebp, esp

	mov eax, 2	; sys_fork
	int 0x80

	pop ebp
	ret
	

_sys_write:

	push ebp
	mov ebp, esp

	mov eax, 4					; sys_write
	mov ebx, 1					; unused
	mov ecx, [ebp + 8]	; msg
	mov edx, [ebp + 12]	; len

	int 0x80

	pop ebp
	ret

_sys_getpid:

	push ebp
	mov ebp, esp

	mov eax, 20					; sys_getpid
	int 0x80

	pop ebp
	ret

