@constants -----------------------------------
.pool

@ bootrom versions :
.set	BOOTROM_OLD,		0x00342e30
.set	BOOTROM_NEW,		0x2e352e30
.set	addr_bootrom_version,	0x288

@ bootrom functions
.set	old_jump_to,                0x3339
.set	old_usb_wait_for_image,	    0x30E9
.set	old_usb_shutdown,	    0x1059
.set	old_aes_crypto_cmd,         0x899

.set	new_jump_to,                0x3341
.set	new_usb_wait_for_image,	    0x30F1
.set	new_usb_shutdown,	    0xDEAD
.set	new_aes_crypto_cmd,         0x899

@ general :
.set	loadaddr,               0x22000000
.set	max_size,               0x24000

@ decryption :
.set	TAG_DATA,               0x44415441
.set	TAG_KBAG,               0x4b424147
.set	AES_TYPE_GID,           0x200
.set	AES_MODE_128,           0x10000000

@ patching related :
.set	search_size_2,          0x200

@@@@@@@ FIX ALL THESE @@@@@@@
.set	FP_RSA_CHECK_1,         0x2aff781a	@ search 1a 78 ff 2a : LDRB R2, [R3] @ CMP R2, #0xff
.set	FP_RSA_CHECK_2,         0x30fff04f	@ search 4f F0 ff 30 : MOV.W R0, #0xffffffff
.set	PATCH_RSA_CHECK,        0x20002000	@ _patch: MOV R0, #0 @ MOV R0, #0

.set	FP_PERMS_CHECK_1,       0xb590dff3	@ search f3 df 90 b5 : F3 DF @ PUSH {R4,R7,LR}
.set	FP_PERMS_CHECK_2,       0x681b4b07	@ search 07 4b 1b 68 : LDR R3, =0x8402dB80 @ LDR R3, [R3]
.set	PATCH_PERMS_CHECK,      0x33fff04f	@ _patch: MOV.W R3, #0xffffffff

.set	FP_IMAGE_VERIFY_TAG_1,  0xf1d39a11	@ search 11 9a d3 f1 : LDR R2, [SP,#0x44] @ D3 F1
.set	FP_IMAGE_VERIFY_TAG_2,  0x2001bf18	@ search 18 bf 01 20 : IT NE @ MOVNE R0, #1
.set	PATCH_IMAGE_VERIFY_TAG, 0x20002000 	@ _patch: MOV R0, #0 @ MOV R0, #0

@ go jump to 0x09000000 :
@ iPod2,1 4.1
                                                @ 64bit search: 80 b5 00 af 82 b0 4f f0
.set    FP_CMD_1,               0xaf00b580      @ search 80 b5 00 af : PUSH {R7,LR} @ ADD R7, SP, #0
.set    FP_CMD_2,               0xf04fb082      @ search 82 b0 4f f0 : SUB SP, SP, #8 @ 4f f0
.set    PATCH_CMD_1,            0x47184b00      @ _patch: LDR R3, =0x09000000 @ BX R3
.set    PATCH_CMD_2,            0x09000000      @ _patch: 0x09000000
@@@@@@@ FIX ALL THESE @@@@@@@

.text
@main code -----------------------------------
.code 16
_start: .global _start
	MOV	R7,	PC
	B	entry_point
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

entry_point:
	@LSR	R7,	#2
	@LSL	R7,	#2
	@ we're gonna detect bootrom version and set the 3 dependencies we need in memory
	LDR	R0,	=addr_bootrom_version
	LDR	R2,	[R0]
	LDR	R0,	=BOOTROM_OLD
	CMP	R0,	R2
	BNE	_not_old
	LDR	R1,	=old_usb_wait_for_image
	STR	R1,	[R7]
	LDR	R1,	=old_aes_crypto_cmd
	STR	R1,	[R7, #4]
	LDR	R1,	=old_jump_to
	STR	R1,	[R7, #8]
	LDR	R1,	=old_usb_shutdown
	MOV	R0,	R7
	ADD	R0,	#0xC
	STR	R1,	[R0]
	B	_wait_for_image

_not_old:
	LDR	R0,	=BOOTROM_NEW
	CMP	R0,	R2
	BNE	_not_new
	LDR	R1,	=new_usb_wait_for_image
	STR	R1,	[R7]
	LDR	R1,	=new_aes_crypto_cmd
	STR	R1,	[R7, #4]
	LDR	R1,	=new_jump_to
	STR	R1,	[R7, #8]
	LDR	R1,	=new_usb_shutdown
	MOV	R0,	R7
	ADD	R0,	#0xC
	STR	R1,	[R0]
	B	_wait_for_image
_not_new:

_wait_for_image:
	MOV	R0,	R7
	ADD	R0,	#0xC
	LDR	R3,	[R0]
	BLX	R3

	LDR	R0,	=loadaddr
	LDR	R1,	=max_size
	LDR	R3,	[R7]
	BLX	R3

	@ decrypt the image
	LDR	R0,	=TAG_DATA
	LDR	R1,	=loadaddr
	LDR	R2,	[R1, #4]	@ header->fullSize
	BL	find
	CMP	R0,	#0
	BEQ	entry_point
	MOV	R6,	R0

	LDR	R0,	=TAG_KBAG
	LDR	R1,	=loadaddr
	LDR	R2,	[R1, #4]	@ header->fullSize
	BL	find
	CMP	R0,	#0
	BEQ	entry_point
	MOV	R5,	R0

	MOV	R0,	#0x11		@ AES_DECRYPT
	MOV	R4,	#0x14
	ADD	R1,	R5,	R4
	ADD	R2,	R5,	R4
	MOV	R3,	#0x30		@ KBAG_KEY_IV_SIZE
	LDR	R4,	=AES_TYPE_GID
	STR	R4,	[SP]
	MOV	R4,	#0
	STR	R4,	[SP, #4]
	MOV	R4,	#0
	STR	R4,	[SP, #8]
	LDR	R4,	[R7, #4]
	BLX	R4

	MOV	R0,	#0x11		@ AES_DECRYPT
	MOV	R4,	#0xc
	ADD	R1,	R6,	R4
	ADD	R2,	R6,	R4
	LDR	R3,	[R6, #0x8]
	LDR	R4,	=AES_MODE_256
	STR	R4,	[SP]
	MOV	R4,	#0x24
	ADD	R4,	R5
	STR	R4,	[SP, #4]
	MOV	R4,	#0x14
	ADD	R4,	R5
	STR	R4,	[SP, #8]
	LDR	R4,	[R7, #4]
	BLX	R4

	@ move image data to loadaddr
	LDR	R0,	=loadaddr
	MOV	R1,	#0xc
	ADD	R1,	R6
	LDR	R2,	[R6, #8]	@ header->fullSize
	MOV	R5,	R2
	BL	memcpy
	
	@ patch the image
	LDR	R0,	=FP_RSA_CHECK_1
	LDR	R1,	=loadaddr
	MOV	R2,	R5
	BL	find
	CMP	R0,	#0
	BEQ	rsa_check_patched
	MOV	R1,	R0
	LDR	R0,	=FP_RSA_CHECK_2
	LDR	R2,	=search_size_2
	BL	find
	CMP	R0,	#0
	BEQ	rsa_check_patched
	LDR	R1,	=PATCH_RSA_CHECK
	STR	R1,	[R0]
rsa_check_patched:

	LDR	R0,	=FP_PERMS_CHECK_1
	LDR     R1,     =loadaddr
        MOV	R2,	R5
	BL	find
	CMP	R0,	#0
	BEQ	perms_check_patched
	MOV	R1,	R0
	LDR	R0,	=FP_PERMS_CHECK_2
	LDR	R2,	=search_size_2
	BL	find
	CMP	R0,	#0
	BEQ	perms_check_patched
	LDR	R1,	=PATCH_PERMS_CHECK
	STR	R1,	[R0]
perms_check_patched:

	LDR	R0,	=FP_IMAGE_VERIFY_TAG_1
	LDR	R1,	=loadaddr
	MOV	R2,	R5
	BL	find
	CMP	R0,	#0
	BEQ	image_verify_tag_patched
	MOV	R1,	R0
	LDR	R0,	=FP_IMAGE_VERIFY_TAG_2
	LDR	R2,	=search_size_2
	BL	find
	CMP	R0,	#0
	BEQ	image_verify_tag_patched
	LDR	R1,	=PATCH_IMAGE_VERIFY_TAG
	STR	R1,	[R0]
image_verify_tag_patched:

	LDR	R0,	=FP_CMD_1
	LDR	R1,	=loadaddr
	MOV	R2,	R5
	LDR	R3,	=FP_CMD_2
	BL	find64
	CMP	R0,	#0
	BNE	bgcolor_patch
	LDR	R0,	=FP_CMD2_1
	LDR	R1,	=loadaddr
	MOV	R2,	R5
	LDR	R3,	=FP_CMD2_2
	BL	find64
	CMP	R0,	#0
	BEQ	bgcolor_patched
bgcolor_patch:
	LDR	R1,	=PATCH_CMD_1
	STR	R1,	[R0]
	LDR	R1,	=PATCH_CMD_2
	STR	R1,	[R0, #4]
bgcolor_patched:

	@ boot that image !
	MOV	R0,	#0
	LDR	R1,	=loadaddr
	MOV	R2,	#0
	LDR	R3,	[R7, #8]
	BLX	R3

	B	entry_point	@ won't happen anyway
@-----------------------------------------------------
memcpy:

_memcpy_loop:
	LDRB	R3,	[R1]
	STRB	R3,	[R0]
	ADD	R0,	#1
	ADD	R1,	#1
	SUB	R2,	#1
	CMP	R2,	#0
	BNE	_memcpy_loop

	BX	LR
@-----------------------------------------------------	
find:
	MOV	R3,	#0
	B	find64

find64:
	PUSH	{R4, LR}
_find_loop:
	LDR	R4,	[R1]
	CMP	R4,	R0
	BNE	_find_loop_continue
	CMP	R3,	#0
	BEQ	_find_ret
	LDR	R4,	[R1, #4]
	CMP	R4,	R3
	BEQ	_find_ret

_find_loop_continue:
	ADD	R1,	#2
	SUB	R2,	#2
	CMP	R2,	#0
	BNE	_find_loop

	MOV	R1,	#0	@ not found

_find_ret:
	MOV	R0,	R1
	POP	{R4, PC}
.end
