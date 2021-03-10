.globl _start

_start:
    b reset

reset:
    mov r0, sp
    mov sp, #0x4000
    add sp, sp, #0x1000

    ldr	r4, =_bstart1
	ldr	r9, =_bend1
	mov	r5, #0
	mov	r6, #0
	mov	r7, #0
	mov	r8, #0

    @ @ enable fpu
    @ mrc p15, 0, r0, c1, c0, 2
    @ orr r0, r0, #0x300000 @ single presicion
    @ orr r0, r0, #0xC00000 @ double precision
    @ mcr p15, 0, r0, c1, c0, 2
    @ mov r0, #0x40000000
    @ fmxr fpexc, r0

    b startup

halt:
	wfe
	b	halt
