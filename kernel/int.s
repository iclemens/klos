	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z15is_install_hookPFvvE
	.align	4, 0x90
__Z15is_install_hookPFvvE:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	call	L1$pb
L1$pb:
	popl	%eax
	movl	8(%ebp), %ecx
	movl	%ecx, -4(%ebp)
	movl	-4(%ebp), %ecx
	movl	%ecx, _sched_hook-L1$pb(%eax)
	addl	$4, %esp
	popl	%ebp
	ret

	.globl	__Z18is_install_handleriPvi
	.align	4, 0x90
__Z18is_install_handleriPvi:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	subl	$12, %esp
	call	L2$pb
L2$pb:
	popl	%eax
	movl	16(%ebp), %ecx
	movl	12(%ebp), %edx
	movl	8(%ebp), %esi
	movl	%esi, -8(%ebp)
	movl	%edx, -12(%ebp)
	movl	%ecx, -16(%ebp)
	movl	-8(%ebp), %ecx
	movl	-12(%ebp), %edx
	movw	%dx, _idt-L2$pb(%eax,%ecx,8)
	movl	-8(%ebp), %ecx
	movw	$8, (_idt-L2$pb)+2(%eax,%ecx,8)
	movl	-8(%ebp), %ecx
	movb	$0, (_idt-L2$pb)+4(%eax,%ecx,8)
	movl	-8(%ebp), %ecx
	movl	-16(%ebp), %edx
	andl	$3, %edx
	shll	$5, %edx
	orb	$142, %dl
	movb	%dl, (_idt-L2$pb)+5(%eax,%ecx,8)
	movl	-8(%ebp), %ecx
	movl	-12(%ebp), %edx
	shrl	$16, %edx
	movw	%dx, (_idt-L2$pb)+6(%eax,%ecx,8)
	addl	$12, %esp
	popl	%esi
	popl	%ebp
	ret

	.globl	_is_cstub
	.align	4, 0x90
_is_cstub:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	call	L3$pb
L3$pb:
	popl	%eax
	leal	L_.str-L3$pb(%eax), %eax
	movl	%eax, (%esp)
	call	__Z15kc_print_stringPKc
	addl	$8, %esp
	popl	%ebp
	ret

	.globl	_is_ckeyboard
	.align	4, 0x90
_is_ckeyboard:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$100, (%esp)
	call	__Z3inbj
	movb	%al, %cl
	movzbl	%cl, %ecx
	movl	%ecx, -4(%ebp)
	movl	$96, (%esp)
	call	__Z3inbj
	movb	%al, %cl
	movzbl	%cl, %ecx
	movl	%ecx, -8(%ebp)
	movl	$32, (%esp)
	movl	$32, 4(%esp)
	call	__Z4outbjj
	addl	$24, %esp
	popl	%ebp
	ret

	.globl	_is_ctimer
	.align	4, 0x90
_is_ctimer:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	call	L5$pb
L5$pb:
	popl	%eax
	movl	__ZZ9is_ctimerE4time-L5$pb(%eax), %ecx
	addl	$10, %ecx
	movl	%ecx, __ZZ9is_ctimerE4time-L5$pb(%eax)
	movl	__ZZ9is_ctimerE4time-L5$pb(%eax), %ecx
	movl	$1746305385, %edx
	movl	%eax, -8(%ebp)
	movl	%ecx, %eax
	mull	%edx
	subl	%edx, %ecx
	shrl	%ecx
	leal	(%ecx,%edx), %eax
	shrl	$7, %eax
	movl	%eax, -4(%ebp)
	movl	%esp, %eax
	movl	$32, 4(%eax)
	movl	$32, (%eax)
	call	__Z4outbjj
	movl	-8(%ebp), %eax
	movl	_sched_hook-L5$pb(%eax), %ecx
	testl	%ecx, %ecx
	je	LBB5_2
	movl	-8(%ebp), %eax
	movl	_sched_hook-L5$pb(%eax), %ecx
	call	*%ecx
LBB5_2:
	addl	$24, %esp
	popl	%ebp
	ret

	.globl	_is_c8259
	.align	4, 0x90
_is_c8259:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	call	L6$pb
L6$pb:
	popl	%eax
	leal	L_.str1-L6$pb(%eax), %eax
	movl	%eax, (%esp)
	call	__Z15kc_print_stringPKc
	movl	$32, (%esp)
	movl	$32, 4(%esp)
	call	__Z4outbjj
	movl	$32, (%esp)
	movl	$160, 4(%esp)
	call	__Z4outbjj
	addl	$8, %esp
	popl	%ebp
	ret

	.globl	_is_cpanic
	.align	4, 0x90
_is_cpanic:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	call	L7$pb
L7$pb:
	popl	%eax
	## InlineAsm Start
	movl %esp, -4(%ebp)
	## InlineAsm End
	leal	L_.str2-L7$pb(%eax), %ecx
	movl	%ecx, (%esp)
	movl	%eax, -12(%ebp)
	call	__Z15kc_print_stringPKc
	movl	-12(%ebp), %eax
	leal	L_.str3-L7$pb(%eax), %eax
	movl	%eax, (%esp)
	call	__Z15kc_print_stringPKc
	movl	-4(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	-8(%ebp), %eax
	movl	40(%eax), %eax
	movl	%eax, (%esp)
	call	__Z14db_stack_tracej
	## InlineAsm Start
	cli
	## InlineAsm End
	## InlineAsm Start
	hlt
	## InlineAsm End
	addl	$24, %esp
	popl	%ebp
	ret

	.globl	_is_capi
	.align	4, 0x90
_is_capi:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	subl	$64, %esp
	call	L8$pb
L8$pb:
	popl	%eax
	movl	20(%ebp), %ecx
	movl	16(%ebp), %edx
	movl	12(%ebp), %esi
	movl	8(%ebp), %edi
	movl	%edi, -12(%ebp)
	movl	%esi, -16(%ebp)
	movl	%edx, -20(%ebp)
	movl	%ecx, -24(%ebp)
	movl	-20(%ebp), %ecx
	movl	%ecx, -36(%ebp)
	movl	-12(%ebp), %ecx
	cmpl	$19, %ecx
	movl	%eax, -48(%ebp)
	movl	%ecx, -52(%ebp)
	jg	LBB8_17
	movl	-52(%ebp), %eax
	leal	-1(%eax), %ecx
	cmpl	$4, %ecx
	movl	%ecx, -56(%ebp)
	ja	LBB8_13
	movl	-56(%ebp), %eax
	movl	%eax, %ecx
	movl	-48(%ebp), %edx
Ltmp0 = LJTI8_0-L8$pb
	movl	Ltmp0(%edx,%ecx,4), %ecx
	leal	(%ecx,%edx), %ecx
	jmpl	*%ecx
LBB8_17:
	movl	-52(%ebp), %eax
	cmpl	$20, %eax
	je	LBB8_12
	jmp	LBB8_13
LBB8_1:
	call	__Z14pr_get_currentv
	movl	%eax, (%esp)
	call	__Z12pr_terminateP12pr_process_t
	movl	-48(%ebp), %eax
	leal	L_.str4-L8$pb(%eax), %ecx
	movl	%ecx, (%esp)
	call	__Z15kc_print_stringPKc
	call	__Z10db_mm_infov
	call	__Z11pr_schedulev
	jmp	LBB8_14
LBB8_2:
	call	__Z14pr_get_currentv
	movl	%eax, (%esp)
	call	__Z15pr_fork_processP12pr_process_t
	movl	%eax, -40(%ebp)
	movl	-48(%ebp), %eax
	leal	L_.str5-L8$pb(%eax), %ecx
	movl	%ecx, (%esp)
	call	__Z15kc_print_stringPKc
	call	__Z14pr_get_currentv
	movl	8(%eax), %eax
	addl	$4096, %eax
	movl	%eax, -44(%ebp)
	movl	-40(%ebp), %eax
	movl	(%eax), %eax
	movl	-44(%ebp), %ecx
	movl	%eax, -24(%ecx)
	movl	-40(%ebp), %eax
	movl	8(%eax), %eax
	addl	$4096, %eax
	movl	%eax, -44(%ebp)
	movl	-44(%ebp), %eax
	movl	$0, -24(%eax)
	movl	-48(%ebp), %eax
	movl	L_pr_state_running$non_lazy_ptr-L8$pb(%eax), %ecx
	movb	(%ecx), %cl
	movl	-40(%ebp), %edx
	movb	%cl, 4(%edx)
	movl	$0, -28(%ebp)
	jmp	LBB8_4
LBB8_3:
	movl	-28(%ebp), %eax
	subl	$17, %eax
	movl	-44(%ebp), %ecx
	movl	$32, (%ecx,%eax,4)
	movl	-28(%ebp), %eax
	addl	$1, %eax
	movl	%eax, -28(%ebp)
LBB8_4:
	movl	-28(%ebp), %eax
	cmpl	$3, %eax
	jbe	LBB8_3
	movl	-40(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z9pr_resumeP12pr_process_t
	jmp	LBB8_14
LBB8_6:
	jmp	LBB8_14
LBB8_7:
	movl	$0, -28(%ebp)
	jmp	LBB8_9
LBB8_8:
	movl	-36(%ebp), %eax
	movl	-28(%ebp), %ecx
	movb	(%eax,%ecx), %al
	movsbl	%al, %eax
	movsbl	%al, %eax
	movl	%eax, (%esp)
	call	__Z18kc_write_characterc
	movl	-28(%ebp), %eax
	addl	$1, %eax
	movl	%eax, -28(%ebp)
LBB8_9:
	movl	-28(%ebp), %eax
	movl	-24(%ebp), %ecx
	cmpl	%ecx, %eax
	jb	LBB8_8
	jmp	LBB8_14
LBB8_11:
	## InlineAsm Start
	mov %eax, -29(%ebp)
	## InlineAsm End
	jmp	LBB8_14
LBB8_12:
	call	__Z9pr_getpidv
	movb	%al, -29(%ebp)
	## InlineAsm Start
	mov %eax, -29(%ebp)
	## InlineAsm End
	jmp	LBB8_14
LBB8_13:
	movl	-48(%ebp), %eax
	leal	L_.str6-L8$pb(%eax), %ecx
	movl	%ecx, (%esp)
	call	__Z15kc_print_stringPKc
	call	_is_cpanic
LBB8_14:
	addl	$64, %esp
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.align	2, 0x90
L8_0_set_1 = LBB8_1-L8$pb
L8_0_set_2 = LBB8_2-L8$pb
L8_0_set_6 = LBB8_6-L8$pb
L8_0_set_7 = LBB8_7-L8$pb
L8_0_set_11 = LBB8_11-L8$pb
LJTI8_0:
	.long	L8_0_set_1
	.long	L8_0_set_2
	.long	L8_0_set_6
	.long	L8_0_set_7
	.long	L8_0_set_11

	.globl	_is_cpic
	.align	4, 0x90
_is_cpic:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	call	L9$pb
L9$pb:
	popl	%eax
	movl	8(%ebp), %ecx
	movl	%ecx, -4(%ebp)
	leal	L_.str7-L9$pb(%eax), %eax
	movl	%eax, (%esp)
	call	__Z15kc_print_stringPKc
	call	_is_cpanic
	addl	$8, %esp
	popl	%ebp
	ret

	.globl	__Z16setup_interruptsv
	.align	4, 0x90
__Z16setup_interruptsv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	subl	$36, %esp
	call	L10$pb
L10$pb:
	popl	%eax
	movl	$0, _sched_hook-L10$pb(%eax)
	movl	$0, -8(%ebp)
	movl	%eax, -16(%ebp)
	jmp	LBB10_2
LBB10_1:
	movl	-8(%ebp), %eax
	movl	-16(%ebp), %ecx
	movl	L_is_panic$non_lazy_ptr-L10$pb(%ecx), %edx
	leal	(%edx), %edx
	movw	%dx, %si
	movw	%si, _idt-L10$pb(%ecx,%eax,8)
	movl	-8(%ebp), %eax
	movw	$8, (_idt-L10$pb)+2(%ecx,%eax,8)
	movl	-8(%ebp), %eax
	movb	$0, (_idt-L10$pb)+4(%ecx,%eax,8)
	movl	-8(%ebp), %eax
	movb	$-114, (_idt-L10$pb)+5(%ecx,%eax,8)
	movl	-8(%ebp), %eax
	shrl	$16, %edx
	movw	%dx, (_idt-L10$pb)+6(%ecx,%eax,8)
	movl	-8(%ebp), %eax
	addl	$1, %eax
	movl	%eax, -8(%ebp)
LBB10_2:
	movl	-8(%ebp), %eax
	cmpl	$255, %eax
	jle	LBB10_1
	movl	-16(%ebp), %eax
	leal	_idt-L10$pb(%eax), %ecx
	movl	%ecx, (_idtp-L10$pb)+2(%eax)
	movw	$2047, _idtp-L10$pb(%eax)
	## InlineAsm Start
	lidt (_idtp-L10$pb(%eax))
	## InlineAsm End
	movl	$255, (%esp)
	movl	$33, 4(%esp)
	call	__Z4outbjj
	movl	$255, (%esp)
	movl	$161, 4(%esp)
	call	__Z4outbjj
	movl	$17, (%esp)
	movl	$32, 4(%esp)
	call	__Z4outbjj
	movl	$32, (%esp)
	movl	$33, 4(%esp)
	call	__Z4outbjj
	movl	$4, (%esp)
	movl	$33, 4(%esp)
	call	__Z4outbjj
	movl	$5, (%esp)
	movl	$33, 4(%esp)
	call	__Z4outbjj
	movl	$17, (%esp)
	movl	$160, 4(%esp)
	call	__Z4outbjj
	movl	$40, (%esp)
	movl	$161, 4(%esp)
	call	__Z4outbjj
	movl	$2, (%esp)
	movl	$161, 4(%esp)
	call	__Z4outbjj
	movl	$1, (%esp)
	movl	$161, 4(%esp)
	call	__Z4outbjj
	movw	$-1, -10(%ebp)
	movl	$54, (%esp)
	movl	$67, 4(%esp)
	call	__Z4outbjj
	movw	-10(%ebp), %ax
	movzwl	%ax, %eax
	andl	$255, %eax
	movl	%eax, (%esp)
	movl	$64, 4(%esp)
	call	__Z4outbjj
	movw	-10(%ebp), %ax
	shrw	$8, %ax
	movzwl	%ax, %eax
	andl	$255, %eax
	movl	%eax, (%esp)
	movl	$64, 4(%esp)
	call	__Z4outbjj
	call	__Z29is_install_exception_handlersv
	movl	-16(%ebp), %eax
	movl	L_is_timer$non_lazy_ptr-L10$pb(%eax), %ecx
	leal	(%ecx), %ecx
	movl	$32, (%esp)
	movl	%ecx, 4(%esp)
	movl	$0, 8(%esp)
	call	__Z18is_install_handleriPvi
	movl	-16(%ebp), %eax
	movl	L_is_keyboard$non_lazy_ptr-L10$pb(%eax), %ecx
	leal	(%ecx), %ecx
	movl	$33, (%esp)
	movl	%ecx, 4(%esp)
	movl	$0, 8(%esp)
	call	__Z18is_install_handleriPvi
	movl	$34, -8(%ebp)
	jmp	LBB10_5
LBB10_4:
	movl	-8(%ebp), %eax
	movl	-16(%ebp), %ecx
	movl	L_is_8259$non_lazy_ptr-L10$pb(%ecx), %edx
	leal	(%edx), %edx
	movl	%eax, (%esp)
	movl	%edx, 4(%esp)
	movl	$0, 8(%esp)
	call	__Z18is_install_handleriPvi
	movl	-8(%ebp), %eax
	addl	$1, %eax
	movl	%eax, -8(%ebp)
LBB10_5:
	movl	-8(%ebp), %eax
	cmpl	$46, %eax
	jle	LBB10_4
	movl	-16(%ebp), %eax
	movl	L_is_api$non_lazy_ptr-L10$pb(%eax), %ecx
	leal	(%ecx), %ecx
	movl	$128, (%esp)
	movl	%ecx, 4(%esp)
	movl	$3, 8(%esp)
	call	__Z18is_install_handleriPvi
	movl	$252, (%esp)
	movl	$33, 4(%esp)
	call	__Z4outbjj
	movl	$255, (%esp)
	movl	$161, 4(%esp)
	call	__Z4outbjj
	## InlineAsm Start
	sti
	## InlineAsm End
	addl	$36, %esp
	popl	%esi
	popl	%ebp
	ret

	.globl	_idt
.zerofill __DATA,__common,_idt,2048,5
	.globl	_sched_hook
.zerofill __DATA,__common,_sched_hook,4,2
	.section	__TEXT,__cstring,cstring_literals
L_.str:
	.asciz	 "\nStub interrupt handler!\n"

.zerofill __DATA,__bss,__ZZ9is_ctimerE4time,4,2
L_.str1:
	.asciz	 "\n8259 Interrupt!\n"

L_.str2:
	.asciz	 "\nKernel panic!\n"

	.align	2
L_.str3:
	.asciz	 "Don't know how to handle interrupt\n"

L_.str4:
	.asciz	 "Process terminated!\n"

L_.str5:
	.asciz	 "Process forked!\n"

L_.str6:
	.asciz	 "Invalid syscall!\n"

	.align	2
L_.str7:
	.asciz	 "\nReceived interrupt from 8259...\n"

	.globl	_idtp
.zerofill __DATA,__common,_idtp,8,3

	.section	__IMPORT,__pointers,non_lazy_symbol_pointers
L_is_8259$non_lazy_ptr:
.indirect_symbol _is_8259
	.long	0
L_is_api$non_lazy_ptr:
.indirect_symbol _is_api
	.long	0
L_is_keyboard$non_lazy_ptr:
.indirect_symbol _is_keyboard
	.long	0
L_is_panic$non_lazy_ptr:
.indirect_symbol _is_panic
	.long	0
L_is_timer$non_lazy_ptr:
.indirect_symbol _is_timer
	.long	0
L_pr_state_running$non_lazy_ptr:
.indirect_symbol _pr_state_running
	.long	0

.subsections_via_symbols
