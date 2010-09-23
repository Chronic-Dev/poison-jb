.arm
.text

.globl exit
.globl fork
.globl read
.globl write
.globl open
.globl close
.globl unlink
.globl chdir

/*
.global _mknod: 0xD  int mknod(const char *pathname, mode_t mode, dev_t dev)                      3
.global _chmod: 0xF  int chmod(const char *path, mode_t mode)                                2
.global _chown: 0x10  int chown(const char *path, uid_t owner, gid_t group)                       3
*/

.globl sync
.globl kill
.globl dup
.globl symlink

/*
.global _execve: 0x3B  int execve(const char *filename, char *const argv[], char *const envp[])   3
*/

.globl chroot
.globl vfork
.globl reboot

/*
.global _munmap: 0x49  int munmap(void *addr, size_t length)                                 2
.global _mprotect: 0x4A  int mprotect(const void *addr, size_t len, int prot)                     3
.global _dup2: 0x5A  int dup2(int oldfd, int newfd)                                          2
*/

.globl mount
.globl stat

/*
.global _mmap: 0xC5  void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) 6
*/

/* void exit(int status) */
exit:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x4
    str    r0, [sp,#0x0]
    mov    r12, #0x1
    swi    #0x80
    add    sp, sp, #0x4
    ldmfd  sp!, {r12,pc}
    
/* pid_t fork(void) */
fork:
    stmfd  sp!, {r12,lr}
    mov    r12, #0x2
    swi    #0x80
    ldmfd  sp!, {r12,pc}
    
/* ssize_t read(int fd, void *buf, size_t count) */
read:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0xC
    str    r0, [sp,#0x8]
    str    r1, [sp,#0x4]
    str    r2, [sp,#0x0]
    mov    r12, #0x3
    swi    #0x80
    add    sp, sp, #0xC
    ldmfd  sp!, {r12,pc}
    
/* ssize_t write(int fd, const void *buf, size_t count) */
write:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0xC
    str    r0, [sp,#0x8]
    str    r1, [sp,#0x4]
    str    r2, [sp,#0x0]
    mov    r12, #0x4
    swi    #0x80
    add    sp, sp, #0xC
    ldmfd  sp!, {r12,pc}
    
/* int open(const char *pathname, int flags, mode_t mode) */
open:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0xC
    str    r0, [sp,#0x8]
    str    r1, [sp,#0x4]
    str    r2, [sp,#0x0]
    mov    r12, #0x5
    swi    #0x80
    add    sp, sp, #0xC
    ldmfd  sp!, {r12,pc}

/* int close(int fd) */
close:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x4
    str    r0, [sp,#0x0]
    mov    r12, #0x6
    swi    #0x80
    add    sp, sp, #0x4
    ldmfd  sp!, {r12,pc}
    
/* int unlink(const char *pathname); */
unlink:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x4
    str    r0, [sp,#0x0]
    mov    r12, #0xA
    swi    #0x80
    add    sp, sp, #0x4
    ldmfd  sp!, {r12,pc}
    
/* int chdir(const char *path) */
chdir:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x4
    str    r0, [sp,#0x0]
    mov    r12, #0xC
    swi    #0x80
    add    sp, sp, #0x4
    ldmfd  sp!, {r12,pc}
    
/* int stat(const char *path, struct stat *buf) */
stat:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x8
    str    r0, [sp,#0x4]
    str    r1, [sp,#0x0]
    mov    r12, #0xBC
    swi    #0x80
    add    sp, sp, #0x8
    ldmfd  sp!, {r12,pc}
    
/* int mount(const char *filesystemtype, const char *target, int mountflags, void *source) */
mount:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x10
    str    r0, [sp,#0xC]
    str    r1, [sp,#0x8]
    str    r2, [sp,#0x4]
    str    r3, [sp,#0x0]
    mov    r12, #0xA7
    swi    #0x80
    add    sp, sp, #0x10
    ldmfd  sp!, {r12,pc}

/* void sync(void) */
sync:
    stmfd  sp!, {r12,lr}
    mov    r12, #0x24 
    swi    #0x80
    ldmfd  sp!, {r12,pc}
    
/* int kill(pid_t pid, int sig) */
kill:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x8
    str    r0, [sp,#0x4]
    str    r1, [sp,#0x0]
    mov    r12, #0x25
    swi    #0x80
    add    sp, sp, #0x8
    ldmfd  sp!, {r12,pc}
    
/* int dup(int oldfd) */
dup:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x4
    str    r0, [sp,#0x0]
    mov    r12, #0x29
    swi    #0x80
    add    sp, sp, #0x4
    ldmfd  sp!, {r12,pc}
    
/* int symlink(const char *oldpath, const char *newpath) */
symlink:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x8
    str    r0, [sp,#0x4]
    str    r1, [sp,#0x0]
    mov    r12, #0x39
    swi    #0x80
    add    sp, sp, #0x8
    ldmfd  sp!, {r12,pc}
    
/* int chroot(const char *path) */
chroot:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x4
    str    r0, [sp,#0x0]
    mov    r12, #0x3D
    swi    #0x80
    add    sp, sp, #0x4
    ldmfd  sp!, {r12,pc}
    
/* pid_t vfork(void) */
vfork:
    stmfd  sp!, {r12,lr}
    mov    r12, #0x42
    swi    #0x80
    ldmfd  sp!, {r12,pc}

/* int reboot(int howto) */
reboot:
    stmfd  sp!, {r12,lr}
    sub    sp, sp, #0x4
    str    r0, [sp,#0x0]
    mov    r12, #0x37
    swi    #0x80
    add    sp, sp, #0x4
    ldmfd  sp!, {r12,pc}
