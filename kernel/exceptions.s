
[extern is_cexception]

%macro EXCEPTION_HANDLER 1
[global is_int%1]
is_int%1:
	pushad

	push ds
	push gs
	push fs
	push es

	; Push error code only for some exceptions
	%if (%1 >= 10 && %1 <= 14) || %1 == 8
	  mov ebx, [esp + 32]
	%elif
	  mov ebx, 0
	%endif

	push ebx

	; Push exception number
	mov ebx, %1
	push ebx

	call is_cexception

	; Clean up the stack
	add esp, 8

	pop es
	pop fs
	pop gs
	pop ds

	popad

	; Remove error code from the stack, if present
	%if (%1 >= 10 && %1 <= 14) || %1 == 8
	add esp, 4
	%endif

	iretd
%endmacro

[global is_int1]
is_int1:
	pushad
;	mov ebx, 1
;	push ebx
;	cli
;	hlt
	popad
	iret

EXCEPTION_HANDLER 0
;EXCEPTION_HANDLER 1
EXCEPTION_HANDLER 2
EXCEPTION_HANDLER 3
EXCEPTION_HANDLER 4
EXCEPTION_HANDLER 5
EXCEPTION_HANDLER 6
EXCEPTION_HANDLER 7
EXCEPTION_HANDLER 8
EXCEPTION_HANDLER 9
EXCEPTION_HANDLER 10
EXCEPTION_HANDLER 11
EXCEPTION_HANDLER 12
EXCEPTION_HANDLER 13
EXCEPTION_HANDLER 14
EXCEPTION_HANDLER 15
EXCEPTION_HANDLER 16
EXCEPTION_HANDLER 17
EXCEPTION_HANDLER 18
EXCEPTION_HANDLER 19
EXCEPTION_HANDLER 20
EXCEPTION_HANDLER 21
EXCEPTION_HANDLER 22
EXCEPTION_HANDLER 23
EXCEPTION_HANDLER 24
EXCEPTION_HANDLER 25
EXCEPTION_HANDLER 26
EXCEPTION_HANDLER 27
EXCEPTION_HANDLER 28
EXCEPTION_HANDLER 29
EXCEPTION_HANDLER 30
EXCEPTION_HANDLER 31
