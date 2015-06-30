[BITS 32]

;
; Low-level interrupt handlers
;
; This code handles interrupts at a very low level. In general, we
; aim to pass control to the C-based kernel as soon as possible.
; Because of the nature of interrupts wrappers are nessecary.
;

; CPU Interrupts
[global is_int0]
[global is_int4]
[global is_int6]
[global is_int7]
[global is_int8]
[global is_int12]
[global is_int13]
[global is_int14]

[global is_stub]

; 8259 Interrupts
[global is_8259]

[global is_timer]
[global is_keyboard]

[extern is_c8259]
[extern is_ctimer]
[extern is_ckeyboard]

; Generic
[global is_panic]
[global is_master_pic]
[global is_slave_pic]

[global is_api]

[extern is_cpanic]
[extern is_cpic]

[extern is_capi]

[extern is_cexception]
[extern is_cstub]

is_stub:
	pushad
	call is_cstub
	popad
	iret

; Interrupt service routine (for unhandled interrupts)
is_panic:
	pushad
	call is_cpanic
	popad
	iret

; 8259 Interrupts
is_8259:
	pushad
	call is_c8259
	popad
	iret

is_timer:
	;cli
	pushad
	call is_ctimer
	popad
	;sti
	iret

is_keyboard:
	pushad
	call is_ckeyboard
	popad
	iret

; Generic
is_master_pic:
	pushad
	call is_cpic
	popad
	iret

is_slave_pic:
	;cli
	pushad
	mov ebx, 0x01
	push ebx
	call is_cpic
	popad
	;sti
	iret


is_api:
	;cli

	push edx
	push ecx
	push ebx
	push eax

	call is_capi	

	; Clean up the stack
	add esp, 16

	;sti

	iretd

