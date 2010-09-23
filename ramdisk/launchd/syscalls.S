.text
.globl _mlock
.globl _stat
.globl _swtch_pri
.globl _mount
.globl _chroot
.globl _chmod
.globl _execve
.globl _reboot


.globl _open
.globl _mkdir
.globl _unmount
.globl _dup2
.globl _write
.globl _read
.globl _close

_open:
	mov r12, #0x5
	swi 0x80
	bx lr

_mkdir:
	mov r12, #0x88
	swi 0x80
	bx lr

_dup2:
	mov r12, #0x5A
	swi 0x80
	bx lr

_write:
	mov r12, #0x4
	swi 0x80
	bx lr

_mlock:
	mov r12, #0xCB
	swi 0x80
	bx lr

_stat:
	mov r12, #0xBC
	swi 0x80
	bx lr

_swtch_pri:
	mvn r12, #0x3a
	swi 0x80
	bx lr

_mount:
	mov r12, #0xA7
	swi 0x80
	bx lr

_unmount:
	mov r12, #0x9F
	swi 0x80
	bx lr

_chroot:
	mov r12, #0x3D
	swi 0x80
	bx lr

_chmod:
	mov r12, #0xF
	swi 0x80
	bx lr

_execve:
	mov r12, #0x3B
	swi 0x80
	bx lr

_reboot:
	mov r12, #0x37
	swi 0x80
	bx lr

_read:
	mov r12, #0x03
	swi 0x80
	bx lr

_close:
	mov r12, #0x06
	swi 0x80
	bx lr

