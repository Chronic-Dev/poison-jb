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
	stmfd  sp!, {r0-r12,lr}
	mov    r0, pc
	lsr    r0, #24
	cmp    r0, #0x41
	bne    go

relocate:
	ldr    r0, =0x84020864
	mov    r1, #0x40000000
	str    r1, [r0]
	mov    r0, #0x41000000
	mov    r1, #0x40000000
	mov    r2, #0x0

loop:
	cmp    r2, #0x100000
	beq    go
	ldr    r3, [r0,r2]
	str    r3, [r1,r2]
	add    r2, #4
	b      loop

go:
	bl     main
	ldmfd  sp!, {r0-r12,pc}

halt:
	b      halt

message; .asciz "relocating payload\n"
