	.file	"main.c"
	.text
	.globl	tutorial3
	.section	.rodata
.LC0:
	.string	"Hello world!"
	.data
	.align 4
	.type	tutorial3, @object
	.size	tutorial3, 4
tutorial3:
	.long	.LC0
	.text
	.globl	in
	.type	in, @function
in:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	movl	8(%ebp), %eax
	movw	%ax, -20(%ebp)
	movzwl	-20(%ebp), %eax
	movl	%eax, %edx
/APP
/  8 "main.c" 1
	in %dx, %al
/  0 "" 2
/NO_APP
	movb	%al, -1(%ebp)
	movzbl	-1(%ebp), %eax
	leave
	ret
	.size	in, .-in
	.globl	out
	.type	out, @function
out:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movw	%dx, -4(%ebp)
	movb	%al, -8(%ebp)
	movzbl	-8(%ebp), %eax
	movzwl	-4(%ebp), %edx
/APP
/  14 "main.c" 1
	out %al, %dx
/  0 "" 2
/NO_APP
	nop
	leave
	ret
	.size	out, .-out
	.globl	print
	.type	print, @function
print:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$753664, -4(%ebp)
	pushl	$14
	pushl	$980
	call	out
	addl	$8, %esp
	pushl	$981
	call	in
	addl	$4, %esp
	movzbl	%al, %eax
	sall	$8, %eax
	movw	%ax, -10(%ebp)
	pushl	$15
	pushl	$980
	call	out
	addl	$8, %esp
	pushl	$981
	call	in
	addl	$4, %esp
	movzbl	%al, %eax
	orw	%ax, -10(%ebp)
	movzwl	-10(%ebp), %eax
	addl	%eax, %eax
	addl	%eax, -4(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L5
.L6:
	movl	-8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -8(%ebp)
	movl	8(%ebp), %edx
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movl	%eax, %edx
	movl	-4(%ebp), %eax
	movb	%dl, (%eax)
	addl	$2, -4(%ebp)
.L5:
	movl	8(%ebp), %edx
	movl	-8(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L6
	movl	-8(%ebp), %eax
	addw	%ax, -10(%ebp)
	pushl	$14
	pushl	$980
	call	out
	addl	$8, %esp
	movzwl	-10(%ebp), %eax
	shrw	$8, %ax
	movzbl	%al, %eax
	pushl	%eax
	pushl	$981
	call	out
	addl	$8, %esp
	pushl	$15
	pushl	$980
	call	out
	addl	$8, %esp
	movzwl	-10(%ebp), %eax
	movzbl	%al, %eax
	pushl	%eax
	pushl	$981
	call	out
	addl	$8, %esp
	nop
	leave
	ret
	.size	print, .-print
	.globl	clrscr
	.type	clrscr, @function
clrscr:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$753664, -4(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L8
.L9:
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -4(%ebp)
	movb	$0, (%eax)
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -4(%ebp)
	movb	$15, (%eax)
	addl	$1, -8(%ebp)
.L8:
	cmpl	$1999, -8(%ebp)
	jle	.L9
	pushl	$14
	pushl	$980
	call	out
	addl	$8, %esp
	pushl	$0
	pushl	$981
	call	out
	addl	$8, %esp
	pushl	$15
	pushl	$980
	call	out
	addl	$8, %esp
	pushl	$0
	pushl	$981
	call	out
	addl	$8, %esp
	nop
	leave
	ret
	.size	clrscr, .-clrscr
	.globl	main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	call	clrscr
	movl	tutorial3, %eax
	pushl	%eax
	call	print
	addl	$4, %esp
.L11:
	jmp	.L11
	.size	main, .-main
	.ident	"GCC: (GNU) 13.2.0"
