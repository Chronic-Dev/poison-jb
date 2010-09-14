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

.set FP_BGCOLOR_1,			0xaf00b580	@ search: PUSH {R7,LR} @ ADD R7, SP, #0
.set FP_BGCOLOR_2,			0x460b2804	@ search: CMP R0, #4 @ MOV R3, R1
.set PATCH_BGCOLOR_1,		0x47184b00	@ _patch: LDR R3, =0x41000000 @ BX R3
.set PATCH_BGCOLOR_2,		0x40000000	@ _patch: 0x40000000

.arm
.ltorg
.code 32
.global _start
_start:
	b		reset
	ldr		pc, undefined_vector
	ldr		pc, syscall_vector
	ldr		pc, prefetch_abort_vector
	ldr		pc, data_abort_vector
	ldr		pc, reserved_vector
	ldr		pc, irq_vector
	ldr		pc, fiq_vector

reset_vector:.word reset
undefined_vector:.word halt
syscall_vector:  .word halt
prefetch_abort_vector:   .word halt
data_abort_vector:   .word halt
reserved_vector: .word halt
irq_vector:  .word halt
fiq_vector:  .word halt

.code 32
reset:
	stmfd  sp!, {r1-r12,lr}
	mov    r4, pc
	lsr    r4, #24
	cmp    r4, #0x41
	beq    go

relocate:
	bl     patch_bgcolor
	mov    r4, #0x41000000
	mov    r5, #0x40000000
	mov    r6, #0x0

relocate_loop:
	cmp    r6, #0x100000
	beq    go
	ldr    r7, [r4,r6]
	str    r7, [r5,r6]
	add    r6, #4
	b      relocate_loop

go:
	bl     main
	ldmfd  sp!, {r1-r12,pc}

halt:
	b      halt

patch_bgcolor:
	stmfd  sp!, {r0-r12,lr}
	ldr    r0, =FP_BGCOLOR_1
	ldr    r1, =TARGET_BASEADDR
	mov    r2, r5
	ldr    r3, =FP_BGCOLOR_2
	bl     find
	cmp    r0, #0
	bne    halt
	ldr    r1, =PATCH_BGCOLOR_1
	str    r1, [r0]
	ldr    r1, =PATCH_BGCOLOR_2
	str    r1, [r0, #4]
	ldmfd  sp!, {r0-r12,pc}

find:
	stmfd  sp!, {r0-r12,lr}

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
	mov	   r0, r1
	ldmfd  sp!, {r0-r12,pc}
