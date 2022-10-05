	.cpu arm7tdmi
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"hoge.cpp"
	.text
	.global	__sync_synchronize
	.section	.text.startup,"ax",%progbits
	.align	2
	.global	main
	.arch armv4t
	.syntax unified
	.arm
	.fpu softvfp
	.type	main, %function
main:
	.fnstart
.LFB336:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	mov	r3, #1
	push	{r4, r5, r6, lr}
	mov	r4, #0
	ldr	r5, .L4
	strb	r3, [r5]
	bl	__sync_synchronize
	strb	r4, [r5]
	bl	__sync_synchronize
	mov	r0, r4
	pop	{r4, r5, r6, lr}
	bx	lr
.L5:
	.align	2
.L4:
	.word	.LANCHOR0
	.cantunwind
	.fnend
	.size	main, .-main
	.global	f
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.type	f, %object
	.size	f, 1
f:
	.space	1
	.ident	"GCC: (GNU) 10.1.0"
