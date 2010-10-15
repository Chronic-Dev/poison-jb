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
.set	FP_CMD_1,	0x47184b00	@ search: LDR R3, =0x41000000 @ BR R3
.set	FP_CMD_2,	0x41000000	@ search: 0x41000000
.set	PATCH_CMD_1,	0x47184b00	@ _patch: LDR R3, =0x42000000 @ BX R3
.set	PATCH_CMD_2,	0x42000000	@ _patch: 0x42000000

.set	FP_CMD2_1,	0x47184b00	@ search: LDR R3, =0x09000000 @ BR R3
.set	FP_CMD2_2,	0x09000000	@ search: 0x09000000
.set	PATCH_CMD2_1,	0x47184b00	@ _patch: LDR R3, =0x0A000000 @ BX R3
.set	PATCH_CMD2_2,	0x0A000000	@ _patch: 0x0A000000

.code 32
@---------------------------------------------
reset:
	push	{r0-r12, lr}
	mov	r0, pc
	lsr	r0, #24
	cmp	r0, #0x42
	beq	relocated

	cmp r0, #0x41
	beq relocate_new

	cmp r0, #0x0A
	beq relocated

	cmp r0, #0x09
	beq	relocate_old

	b halt

relocate_new:
	ldr r4, =0x41000000
	ldr r5, =0x42000000
	b relocate

relocate_old:
	ldr r4, =0x09000000
	ldr r5, =0x0A000000
	b relocate
	
relocate:
	mov r0, r5
	mov r1, #0
	mov r2, #0x80000
	bl  memset
	bl	patch
	mov r0, r4
	mov r1, r5
	bl	copy
	@bl	clear_icache
	@bl	flush_dcache

	mov	r0, r5
	blx	r0
	b	done

relocated:
	ldr	r0, [sp, #0x0]
	ldr	r1, [sp, #0x4]
	ldr	r2, [sp, #0x8]
	ldr	r3, [sp, #0xc]
	bl	main

done:
	pop	{r0-r12, pc}

halt:
	b      halt

@---------------------------------------------
.code 32
copy:
	mov	r2, #0x80000
copy_loop:
	ldr	r3, [r0], #4
	str	r3, [r1], #4
	subs r2, r2, #4
	bne	copy_loop
	bx	lr

@---------------------------------------------
.code 32
patch:
	push {r4,r5,lr}
	mov	r4, pc
	lsr	r4, #24

patch1:
	cmp r4, #0x41
	bne patch2
	ldr	r0, =FP_CMD_1
	ldr r1, =TARGET_BASEADDR
	mov	r2, #0x2c000		@ search area
	ldr	r3, =FP_CMD_2
	bl	find_64
	cmp	r0, #0
	beq	patch_done
	ldr	r1, =PATCH_CMD_1
	str	r1, [r0]
	ldr	r1, =PATCH_CMD_2
	str	r1, [r0, #4]
	b patch_done

patch2:
	cmp r4, #0x9
	bne	patch3
	ldr	r0, =FP_CMD2_1
	ldr r1, =TARGET_BASEADDR
	mov	r2, #0x2c000		@ search area
	ldr	r3, =FP_CMD2_2
	bl	find_64
	cmp	r0, #0
	beq	patch_done
	ldr	r1, =PATCH_CMD2_1
	str	r1, [r0]
	ldr	r1, =PATCH_CMD2_2
	str	r1, [r0, #4]
	b patch_done

patch3:
	b halt

patch_done:
	pop	{r4,r5,pc}

@---------------------------------------------
.code 32
find_64:
	push	{r4, lr}

find_loop:
	ldr    r4, [r1]
	cmp    r4, r0
	bne    find_loop_continue
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
	pop 	{r4, pc}

@---------------------------------------------
.code 32
clear_icache:
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5
	nop
	nop
	nop
	nop
	bx	lr

@---------------------------------------------
.code 32
flush_dcache:
	mrc	p15, 0, r0, c1, c0, 1
	bx	lr
@---------------------------------------------

