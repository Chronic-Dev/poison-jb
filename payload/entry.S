/**
  * GreenPois0n Cynanide - entry.s
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "offsets.h"

.arm
.ltorg
.code 32
.global _start
_start:
	b		reset
	ldr	pc,	undefined_vector
	ldr	pc,	syscall_vector
	ldr	pc,	prefetch_abort_vector
	ldr	pc,	data_abort_vector
	ldr	pc,	reserved_vector
	ldr	pc,	irq_vector
	ldr	pc,	fiq_vector

reset_vector:		.word reset
undefined_vector:	.word halt
syscall_vector:		.word halt
prefetch_abort_vector:	.word halt
data_abort_vector:	.word halt
reserved_vector:	.word halt
irq_vector:		.word halt
fiq_vector:		.word halt

.pool
@ iPhone3,1, iPod4,1 - 4.0.x & 4.1
.set	FP_BGCOLOR_1,		0xaf00b580	@ search: PUSH {R7,LR} @ ADD R7, SP, #0
.set	FP_BGCOLOR_2,		0x460b2804	@ search: CMP R0, #4 @ MOV R3, R1
@ iPad1,1 3.2.x:
.set    FP_BGCOLOR2_1,		0xaf01b590      @ search: PUSH {R4,R7,LR} @ ADD R7, SP, #4
.set    FP_BGCOLOR2_2,		0x460c2804      @ search: CMP R0, #4 @ MOV R4, R1
.set	PATCH_BGCOLOR_1,	0x47184b00	@ _patch: LDR R3, =0x40000000 @ BX R3
.set	PATCH_BGCOLOR_2,	0x40000000	@ _patch: 0x40000000

.code 32
@---------------------------------------------
reset:
	stmfd	sp!, {r0-r12,lr}
	mov	r0, pc
	lsr	r0, #24
	cmp	r0, #0x40
	beq	relocated

	bl	patch_bgcolor
	bl	relocate
	bl	clear_icache
	bl	flush_dcache
	mov	r0, #0x40000000	@ recursive call the relocated reset()
	blx	r0
	b	done

relocated:
	bl     main
done:
	ldmfd  sp!, {r0-r12,pc}

halt:
	b      halt

@---------------------------------------------
relocate:
	mov	r0, #0x41000000
	mov	r1, #0x40000000
	mov	r2, #0x00100000
relocate_loop:
	ldr	r3, [r0], #4
	str	r3, [r1], #4
	subs	r2, r2, #4
	bne	relocate_loop
	bx	lr

@---------------------------------------------
patch_bgcolor:
	ldr	r0, =FP_BGCOLOR_1
	mov	r1, #0x41000000
	mov	r2, #0x00100000		@ search area
	ldr	r3, =FP_BGCOLOR_2
	bl	find
	cmp	r0, #0
	bne	halt
	ldr	r1, =PATCH_BGCOLOR_1
	str	r1, [r0]
	ldr	r1, =PATCH_BGCOLOR_2
	str	r1, [r0, #4]
	bx	lr

@---------------------------------------------
find:
	stmfd  sp!, {r4,lr}

find_loop:
	ldr    r4, [r1]
	cmp    r4, r0
	bne    find_loop_continue
	cmp    r3, #0
	beq    find_return
	ldr    r4, [r1,#4]
	cmp    r4, r3
	beq    find_return

find_loop_continue:
	add    r1, #2
	sub    r2, #2
	cmp    r2, #0
	bne    find_loop
	mov    r1, #0

find_return:
	mov	r0, r1
	ldmfd 	sp!, {r4,pc}

@---------------------------------------------
clear_icache:
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5
	nop
	nop
	nop
	nop
	bx	lr

@---------------------------------------------
flush_dcache:
	mrc	p15, 0, r0, c1, c0, 1
	bx	lr
@---------------------------------------------

